import subprocess
import json
import os
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# Path to the compiled C++ executable
EXE_PATH = os.path.join(os.path.dirname(__file__), 'build', 'Release', 'GenomicSearchEngine.exe')

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/search', methods=['POST'])
def search():
    data = request.json
    pattern = data.get('pattern', '')
    
    if not pattern:
        return jsonify({'error': 'Empty pattern'}), 400

    try:
        # Call the C++ executable with --json and pattern arguments
        # We set cwd to the build directory so it finds synthetic_dna.txt properly
        cwd = os.path.dirname(EXE_PATH)
        result = subprocess.run(
            [EXE_PATH, '--json', pattern],
            cwd=cwd,
            capture_output=True,
            text=True,
            check=True
        )
        # Parse the JSON output from stdout
        response_data = json.loads(result.stdout)
        
        # We'll also fetch the first 1000 characters of the DNA file to show on the UI context
        dna_path = os.path.join(cwd, 'synthetic_dna.txt')
        context = ""
        if os.path.exists(dna_path):
            with open(dna_path, 'r') as f:
                context = f.read()
        
        response_data['dna_context'] = context
        return jsonify(response_data)
    except subprocess.CalledProcessError as e:
        return jsonify({'error': 'Backend process failed', 'details': e.stderr}), 500
    except json.JSONDecodeError as e:
        return jsonify({'error': 'Failed to parse JSON from backend', 'details': str(e)}), 500
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True, port=5000)
