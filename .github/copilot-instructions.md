# Copilot / AI Agent Instructions for AI_STUDIO

Short, actionable guidance to help an AI agent be productive in this repo.

- Repo layout: single-script demo. Key file: `test.py` (root). The script demonstrates calling Google GenAI (`google.genai`) to run inference against Gemini.

- Entry point: `test.py` -- run with Python in the workspace root:

  ```powershell
  # set Gemin API key for current PowerShell session
  $env:GEMINI_API_KEY = "<YOUR_API_KEY>"
  python .\test.py
  ```

- Dependencies: the script uses Google GenAI client. Install with pip if missing:

  ```powershell
  pip install google-genai
  ```

- Environment variables:
  - `GEMINI_API_KEY`: required by `genai.Client()` (client auto-reads this env var). If initialization fails, `test.py` prints the exception and exits.

- Important code patterns and conventions (from `test.py`):
  - Client instantiation uses `client = genai.Client()` and expects the environment variable to be set rather than passing an explicit key in code.
  - Model call pattern:
    ```python
    response = client.models.generate_content(
        model='gemini-2.5-flash',
        contents='...prompt...',
        config=types.GenerateContentConfig(temperature=0.0)
    )
    print(response.text)
    ```
    - The project explicitly sets `temperature=0.0` for deterministic, technical answers. Preserve this when replicating existing behavior.
  - Error handling: client initialization is wrapped in try/except and exits on failure.

- Architecture & "why":
  - This is a minimal integration demo: local script -> Google GenAI model. The main design choice is to rely on environment config for secrets and keep the example focused on model invocation and deterministic config.

- When suggesting code changes or new files, follow these rules:
  - Do not commit secrets or paste API keys into the repo.
  - Keep example prompts and tests idempotent (use fixed seeds/temperature as shown).
  - If adding new dependencies, update a `requirements.txt` or `pyproject.toml` and include an install command in the README.

- Suggested developer workflow (Windows PowerShell):
  - Create venv, install deps, run:
    ```powershell
    python -m venv .venv; .\.venv\Scripts\Activate.ps1; pip install -U pip; pip install google-genai; $env:GEMINI_API_KEY="<KEY>"; python .\test.py
    ```

- Examples to reference in changes:
  - `test.py` demonstrates environment-driven client setup, deterministic config (`temperature=0.0`), and printing `response.text`.

- Missing / unknowns (request user input):
  - No tests, CI, or additional source files found. If there are hidden folders or other services, tell the agent where to look (folders, service names, or Docker files).
  - Provide preferred Python version and any corporate proxy configuration if present.

- If you need to extend this repo (add tests or CI):
  - Keep one small unit test that mocks `genai.Client` calls (avoid real API calls in CI).
  - Add a `requirements.txt` and a minimal GitHub Actions workflow that runs the test and does not send secrets to logs.

Please ask the repo owner for any missing integration details (CI commands, target Python version, or additional services).