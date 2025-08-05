from flask import Flask, request, jsonify
import wave
import os
from vosk import Model, KaldiRecognizer

app = Flask(__name__)

# Load the model only once when the server starts
model_path = "model"
if not os.path.exists(model_path):
    raise FileNotFoundError("Vosk model not found in 'model' directory")
model = Model(model_path)

@app.route('/transcribe', methods=['POST'])
def transcribe():
    try:
        raw_data = request.data
        if not raw_data:
            return jsonify({"error": "No audio data received"}), 400

        temp_file = "temp.wav"

        # Save the raw PCM data as a proper WAV file
        with wave.open(temp_file, "wb") as wf:
            wf.setnchannels(1)           # Mono
            wf.setsampwidth(2)           # 16-bit audio = 2 bytes
            wf.setframerate(16000)       # 16 kHz sampling rate
            wf.writeframes(raw_data)

        # Transcribe with Vosk
        recognizer = KaldiRecognizer(model, 16000)

        with open(temp_file, "rb") as f:
            while True:
                data = f.read(4000)
                if len(data) == 0:
                    break
                recognizer.AcceptWaveform(data)

        result = recognizer.FinalResult()
        os.remove(temp_file)

        return jsonify({"result": result}), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=3000)
