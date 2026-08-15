# Development Guide

## Local Environment Setup

```bash
git clone https://github.com/GizzZmo/CyberAsio-Core.git
cd CyberAsio-Core
npm install
npm run dev
```

### Recommended Tools

- VS Code + Angular Language Service
- Chrome DevTools (Performance + Web Audio tab)
- Playwright for local visual testing

## Coding Standards

- Prefer Angular **signals** and **computed** over classic RxJS subjects for UI state.
- Keep components standalone.
- Use OnPush + zoneless where possible.
- Tailwind classes for layout and theming; avoid large custom CSS files unless necessary.
- Type everything; avoid `any`.

## Adding a New Visualization Mode

1. Extend the `VisualizationType` union.
2. Add a button in the template.
3. Implement `drawNewMode()` using the appropriate analyser data.
4. Call it from the `runVisualizer` switch.
5. Update documentation and (optionally) the screenshot workflow.

## Testing Strategy (Current & Planned)

- Manual interactive testing remains primary while the project is in early stages.
- CI already builds the production bundle on every PR.
- Screenshot job provides visual baselines.
- Future: unit tests for latency calculation and pure functions; Playwright e2e for critical flows.

## Working with the C++ Backend

Until source is published:

- Treat `cpp-backend/build/cyberasio-core` as a black-box reference.
- Do not commit additional CMake cache or object files.
- When source arrives, add a `CMakeLists.txt` at the root of `cpp-backend/` and a corresponding CI job.

## Pull Request Checklist

- [ ] Code builds (`npm run build`)
- [ ] No new console errors in the browser
- [ ] Documentation updated if behavior or public API changed
- [ ] Screenshots regenerated if UI changed significantly (run the workflow or local capture)
- [ ] CI green
