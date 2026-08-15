# CI, Artifacts & Screenshot Generation

This document describes the continuous integration system that produces build artifacts and visual assets for CyberASIO Core.

## Workflows

### 1. `ci.yml` — Build & Artifact Upload

**Trigger**: `push` and `pull_request` to `main` (and other branches).

**Jobs**:

| Job            | Description                                      |
|----------------|--------------------------------------------------|
| `build`        | Installs Node, runs `npm ci` + `npm run build`, uploads `./dist` as artifact named `cyberasio-core-dist` |

**Artifacts**:

- Name: `cyberasio-core-dist`
- Retention: 30 days
- Contents: production Angular build (static files)

Download from the Actions run summary page.

### 2. `screenshots.yml` — UI Screenshot Generation

**Trigger**:

- `push` to `main`
- `workflow_dispatch` (manual)
- Optional schedule (commented out by default)

**Purpose**: Produce high-quality screenshots of the control panel in different states for documentation, README, and visual regression baselines.

**How it works**:

1. Checkout + Node setup
2. `npm ci`
3. Install Playwright browsers
4. Build the app (`npm run build`)
5. Serve the production build with a simple static server
6. Launch Playwright, navigate to the app, interact (select visualization modes, etc.)
7. Capture full-page and component screenshots
8. Upload screenshots as workflow artifacts (`ui-screenshots`)
9. (Optional future step) Commit updated screenshots into `docs/assets/` on main

**Artifacts**:

- Name: `ui-screenshots`
- Files typically include:
  - `overview.png`
  - `bars-mode.png`
  - `spectrogram-mode.png`
  - `waveform-mode.png`
  - `settings-panel.png`

## Using the Artifacts

1. Go to the **Actions** tab of the repository.
2. Select a successful run.
3. Scroll to **Artifacts** at the bottom.
4. Download the zip and extract.

You can then place the images into `docs/assets/` or use them in blog posts, PRs, etc.

## Extending the Pipeline

### Adding more screenshots

Edit `.github/workflows/screenshots.yml` and the Playwright script (usually under `scripts/capture-screenshots.mjs` or inline).  
Add new `page.screenshot()` calls for additional states (different devices selected, extreme buffer sizes, dark/light if added later).

### Visual regression

Once baselines exist in the repo, you can add a comparison step with `pixelmatch` or Playwright’s built-in snapshot testing and fail the job on significant differences.

### Packaging the C++ binary

When source is available, add a matrix job that builds the native binary on Windows (and optionally Linux) and uploads it as an additional artifact.

### Multi-browser screenshots

Extend the Playwright job with a strategy matrix for `chromium`, `firefox`, and `webkit`.

## Local Screenshot Generation

You can run the same capture logic locally:

```bash
npm run build
npx serve dist -p 4173 &
npx playwright install chromium
node scripts/capture-screenshots.mjs   # once the script is added
```

(Exact script path may vary; check the workflow for the current implementation.)

## Best Practices

- Keep screenshot resolution consistent (e.g. 1280×800 or 1920×1080).
- Prefer full-page captures for documentation.
- Name files descriptively and keep them under version control only when they are intentional documentation assets.
- Do not commit large binary artifacts from `cpp-backend/build` long-term; prefer CI-generated packages.
