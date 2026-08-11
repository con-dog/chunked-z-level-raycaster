const pageUrl = process.argv[2] ?? "http://127.0.0.1:8000/raycaster.html";
const devtoolsUrl = process.argv[3] ?? "http://127.0.0.1:9222";

const delay = (milliseconds) => new Promise((resolve) => setTimeout(resolve, milliseconds));

async function fetchJson(url, options) {
  const response = await fetch(url, options);
  if (!response.ok) {
    throw new Error(`${response.status} ${response.statusText}: ${url}`);
  }
  return response.json();
}

const target = await fetchJson(
  `${devtoolsUrl}/json/new?${encodeURIComponent("about:blank")}`,
  { method: "PUT" },
);

const socket = new WebSocket(target.webSocketDebuggerUrl);
await new Promise((resolve, reject) => {
  socket.addEventListener("open", resolve, { once: true });
  socket.addEventListener("error", reject, { once: true });
});

let messageId = 0;
const pending = new Map();
const browserErrors = [];

socket.addEventListener("message", (event) => {
  const message = JSON.parse(event.data);
  if (message.id) {
    const request = pending.get(message.id);
    if (!request) return;
    pending.delete(message.id);
    if (message.error) request.reject(new Error(message.error.message));
    else request.resolve(message.result);
    return;
  }

  if (message.method === "Runtime.exceptionThrown") {
    browserErrors.push(message.params.exceptionDetails.text);
  }
  if (
    message.method === "Runtime.consoleAPICalled" &&
    (message.params.type === "error" || message.params.type === "assert")
  ) {
    browserErrors.push(
      message.params.args.map((argument) => argument.value ?? argument.description).join(" "),
    );
  }
});

function command(method, params = {}) {
  const id = ++messageId;
  socket.send(JSON.stringify({ id, method, params }));
  return new Promise((resolve, reject) => pending.set(id, { resolve, reject }));
}

function canvasIsReady(state) {
  return (
    state?.readyState === "complete" &&
    state.statusHidden &&
    state.canvasWidth >= 320 &&
    state.canvasHeight >= 160 &&
    Math.abs(state.canvasWidth / state.canvasHeight - 2) < 0.02
  );
}

await Promise.all([
  command("Page.enable"),
  command("Runtime.enable"),
]);
await command("Page.navigate", { url: pageUrl });

let pageState;
for (let attempt = 0; attempt < 50; attempt++) {
  await delay(200);
  const evaluation = await command("Runtime.evaluate", {
    expression: `(() => {
      const canvas = document.querySelector("#canvas");
      const status = document.querySelector("#status");
      return {
        readyState: document.readyState,
        statusHidden: Boolean(status?.hidden),
        canvasWidth: canvas?.width ?? 0,
        canvasHeight: canvas?.height ?? 0,
      };
    })()`,
    returnByValue: true,
  });
  pageState = evaluation.result.value;
  if (canvasIsReady(pageState)) {
    break;
  }
}

await command("Input.dispatchKeyEvent", {
  type: "keyDown",
  key: "ArrowRight",
  code: "ArrowRight",
  windowsVirtualKeyCode: 39,
});
await delay(250);
await command("Input.dispatchKeyEvent", {
  type: "keyUp",
  key: "ArrowRight",
  code: "ArrowRight",
  windowsVirtualKeyCode: 39,
});

const screenshot = await command("Page.captureScreenshot", { format: "png" });
const passed =
  canvasIsReady(pageState) &&
  screenshot.data.length > 2000 &&
  browserErrors.length === 0;

console.log(JSON.stringify({ passed, pageState, screenshotBytes: screenshot.data.length * 0.75, browserErrors }, null, 2));

socket.close();
await fetch(`${devtoolsUrl}/json/close/${target.id}`);

if (!passed) process.exitCode = 1;
