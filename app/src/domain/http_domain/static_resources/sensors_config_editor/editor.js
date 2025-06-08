function escapeHTML(str) {
    return str.replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;");
}

function highlight(text) {
    try {
        const json = JSON.parse(text);
        const pretty = JSON.stringify(json, null, 2);
        return escapeHTML(pretty).replace(
            /("(\\u[a-zA-Z0-9]{4}|\\[^u]|[^\\"])*"(\s*:)?|\b(true|false|null)\b|-?\d+(\.\d+)?([eE][+-]?\d+)?)/g,
            match => {
                let cls = "number";
                if (/^"/.test(match)) {
                    cls = /:$/.test(match) ? "key" : "string";
                } else if (/true|false/.test(match)) {
                    cls = "boolean";
                } else if (/null/.test(match)) {
                    cls = "null";
                }
                return `<span class="${cls}">${match}</span>`;
            }
        );
    } catch {
        return `<span class="error">${escapeHTML(text)}</span>`;
    }
}

function getCaretCharacterOffsetWithin(element) {
    const selection = window.getSelection();
    let caretOffset = 0;
    if (selection.rangeCount > 0) {
        const range = selection.getRangeAt(0);
        const preCaretRange = range.cloneRange();
        preCaretRange.selectNodeContents(element);
        preCaretRange.setEnd(range.endContainer, range.endOffset);
        caretOffset = preCaretRange.toString().length;
    }
    return caretOffset;
}

function setCaretPosition(element, offset) {
    const range = document.createRange();
    const sel = window.getSelection();
    let current = 0;

    function traverse(node) {
        if (node.nodeType === Node.TEXT_NODE) {
            const next = current + node.length;
            if (current <= offset && offset <= next) {
                range.setStart(node, offset - current);
                range.collapse(true);
                sel.removeAllRanges();
                sel.addRange(range);
                return true;
            }
            current = next;
        } else {
            for (const child of node.childNodes) {
                if (traverse(child)) return true;
            }
        }
        return false;
    }

    traverse(element);
}

const undoStack = [];
const redoStack = [];
const maxStackSize = 100;

function pushUndoState(text) {
    undoStack.push(text);
    if (undoStack.length > maxStackSize) {
        undoStack.shift();
    }
}

function onEdit() {
    const editor = document.getElementById("editor");
    const raw = editor.innerText;
    const caretOffset = getCaretCharacterOffsetWithin(editor);

    pushUndoState(lastText); // Save previous state for undo
    redoStack.length = 0; // Clear redo stack on new input

    lastText = raw;
    editor.innerHTML = highlight(raw);
    setCaretPosition(editor, caretOffset);
}

document.addEventListener("keydown", (e) => {
    if (e.ctrlKey && e.key === "z" && !e.shiftKey) {
        e.preventDefault();
        undo();
    } else if (e.ctrlKey && e.key === "Z" || (e.ctrlKey && e.shiftKey && e.key === "z")) {
        e.preventDefault();
        redo();
    }
});

function undo() {
    if (undoStack.length === 0) return;
    const editor = document.getElementById("editor");
    const currentText = editor.innerText;
    redoStack.push(currentText);
    const previous = undoStack.pop();
    lastText = previous;
    editor.innerHTML = highlight(previous);
    setCaretPosition(editor, previous.length);
}

function redo() {
    if (redoStack.length === 0) return;
    const editor = document.getElementById("editor");
    const currentText = editor.innerText;
    undoStack.push(currentText);
    const next = redoStack.pop();
    lastText = next;
    editor.innerHTML = highlight(next);
    setCaretPosition(editor, next.length);
}

async function loadConfig() {
    const loader = document.getElementById("loader");
    const editor = document.getElementById("editor");
    loader.style.display = "block";
    editor.style.visibility = "hidden";

    const res = await fetch('/config/sensors');
    const data = await res.json();
    const text = JSON.stringify(data, null, 2);
    lastText = text;
    editor.innerHTML = highlight(text);

    loader.style.display = "none";
    editor.style.visibility = "visible";
}

async function saveConfig() {
    const raw = document.getElementById("editor").innerText;
    try {
        JSON.parse(raw);
    } catch (e) {
        alert("Invalid JSON");
        return;
    }

    await fetch("/config/sensors", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: raw
    });

    alert("Saved!");
}

// Loader dot animation
let dotCount = 0;
setInterval(() => {
    const dots = document.getElementById("dots");
    dotCount = (dotCount + 1) % 4;
    dots.textContent = '.'.repeat(dotCount);
}, 400);

let lastText = "";
loadConfig();
