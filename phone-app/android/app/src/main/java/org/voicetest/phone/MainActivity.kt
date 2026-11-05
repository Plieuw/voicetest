package org.voicetest.phone

import android.Manifest
import android.app.Activity
import android.content.pm.PackageManager
import android.media.AudioFormat
import android.media.AudioRecord
import android.media.MediaRecorder
import android.os.Bundle
import android.view.Gravity
import android.widget.Button
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.TextView
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetAddress
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.util.concurrent.atomic.AtomicBoolean
import kotlin.concurrent.thread

class MainActivity : Activity() {
    companion object {
        private const val SAMPLE_RATE = 48_000
        private const val SAMPLES_PER_PACKET = 960
        private const val PORT = 39000
        private const val HEADER_SIZE = 20
        private const val REQUEST_MIC = 1001
    }

    private lateinit var addressInput: EditText
    private lateinit var statusText: TextView
    private lateinit var startButton: Button
    private lateinit var stopButton: Button
    private val running = AtomicBoolean(false)
    @Volatile private var worker: Thread? = null
    @Volatile private var recorder: AudioRecord? = null
    @Volatile private var socket: DatagramSocket? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        addressInput = EditText(this).apply {
            hint = "FM1 IPv4 address"
            setSingleLine(true)
            setText("192.168.1.100")
        }
        statusText = TextView(this).apply {
            text = "Stopped — enter the FM1 IP address"
            textSize = 16f
            gravity = Gravity.CENTER
        }
        startButton = Button(this).apply {
            text = "Start microphone stream"
            setOnClickListener { requestAndStart() }
        }
        stopButton = Button(this).apply {
            text = "Stop"
            isEnabled = false
            setOnClickListener { stopStreaming() }
        }
        val layout = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            gravity = Gravity.CENTER
            setPadding(32, 48, 32, 32)
            addView(TextView(this@MainActivity).apply {
                text = "VoiceTest Phone Link"
                textSize = 22f
            })
            addView(addressInput)
            addView(statusText)
            addView(startButton)
            addView(stopButton)
        }
        setContentView(layout)
    }

    private fun requestAndStart() {
        if (checkSelfPermission(Manifest.permission.RECORD_AUDIO) !=
            PackageManager.PERMISSION_GRANTED) {
            requestPermissions(arrayOf(Manifest.permission.RECORD_AUDIO), REQUEST_MIC)
            return
        }
        startStreaming()
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_MIC &&
            grantResults.isNotEmpty() &&
            grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            startStreaming()
        } else if (requestCode == REQUEST_MIC) {
            statusText.text = "Microphone permission is required to stream"
        }
    }

    private fun startStreaming() {
        if (running.get()) return
        val host = addressInput.text.toString().trim()
        val target = try {
            InetAddress.getByName(host)
        } catch (_: Exception) {
            statusText.text = "Enter a valid FM1 IPv4 address"
            return
        }
        val minBuffer = AudioRecord.getMinBufferSize(
            SAMPLE_RATE, AudioFormat.CHANNEL_IN_MONO,
            AudioFormat.ENCODING_PCM_16BIT
        )
        if (minBuffer <= 0) {
            statusText.text = "This phone does not support the requested audio format"
            return
        }
        val audio = try {
            AudioRecord(
                MediaRecorder.AudioSource.MIC, SAMPLE_RATE,
                AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
                maxOf(minBuffer, SAMPLES_PER_PACKET * 2 * 4)
            )
        } catch (_: Exception) {
            statusText.text = "Could not open the microphone"
            return
        }
        if (audio.state != AudioRecord.STATE_INITIALIZED) {
            audio.release()
            statusText.text = "Microphone initialisation failed"
            return
        }
        val udp = try {
            DatagramSocket()
        } catch (_: Exception) {
            audio.release()
            statusText.text = "Could not open UDP socket"
            return
        }

        recorder = audio
        socket = udp
        running.set(true)
        addressInput.isEnabled = false
        startButton.isEnabled = false
        stopButton.isEnabled = true
        statusText.text = "Streaming microphone to $host:$PORT"

        worker = thread(name = "VoiceTestAudioCapture") {
            val pcm = ShortArray(SAMPLES_PER_PACKET)
            val packetBytes = ByteArray(HEADER_SIZE + SAMPLES_PER_PACKET * 2)
            var sequence = 0L
            var sampleCounter = 0L
            val header = ByteBuffer.wrap(packetBytes).order(ByteOrder.LITTLE_ENDIAN)
            try {
                audio.startRecording()
                while (running.get()) {
                    var offset = 0
                    while (offset < SAMPLES_PER_PACKET && running.get()) {
                        val count = audio.read(
                            pcm, offset, SAMPLES_PER_PACKET - offset,
                            AudioRecord.READ_BLOCKING
                        )
                        if (count < 0) throw IllegalStateException("AudioRecord read error: $count")
                        if (count == 0) continue
                        offset += count
                    }
                    if (!running.get()) break

                    header.position(0)
                    header.put(byteArrayOf(0x56, 0x54, 0x50, 0x31))
                    header.put(1)
                    header.put(0)
                    header.putShort(HEADER_SIZE.toShort())
                    header.putInt(sequence.toInt())
                    header.putLong(sampleCounter)
                    var p = HEADER_SIZE
                    for (sample in pcm) {
                        packetBytes[p++] = (sample.toInt() and 0xff).toByte()
                        packetBytes[p++] = ((sample.toInt() ushr 8) and 0xff).toByte()
                    }
                    udp.send(DatagramPacket(packetBytes, packetBytes.size, target, PORT))
                    sequence = (sequence + 1L) and 0xffff_ffffL
                    sampleCounter += SAMPLES_PER_PACKET
                }
            } catch (e: Exception) {
                runOnUiThread {
                    statusText.text = "Stream stopped: ${e.message ?: "network/audio error"}"
                }
            } finally {
                try { audio.stop() } catch (_: Exception) {}
                audio.release()
                udp.close()
                running.set(false)
                recorder = null
                socket = null
                runOnUiThread {
                    addressInput.isEnabled = true
                    startButton.isEnabled = true
                    stopButton.isEnabled = false
                    if (statusText.text.startsWith("Streaming")) statusText.text = "Stopped"
                }
            }
        }
    }

    private fun stopStreaming() {
        running.set(false)
        try { recorder?.stop() } catch (_: Exception) {}
        try { socket?.close() } catch (_: Exception) {}
        worker?.interrupt()
        statusText.text = "Stopping…"
    }

    override fun onDestroy() {
        stopStreaming()
        super.onDestroy()
    }
}
