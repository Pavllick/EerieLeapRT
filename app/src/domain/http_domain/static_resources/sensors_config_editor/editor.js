function escapeHTML(str) {
    return str.replace(/&/g, "&amp;").replace(/</g, "&lt;");
}

function update(text) {
    if (text.endsWith("\n")) text += " ";
    const el = document.getElementById("highlighting-content");
    el.innerHTML = escapeHTML(text);
    Prism.highlightElement(el);
}

function syncScroll(el) {
    const h = document.getElementById("highlighting");
    h.scrollTop = el.scrollTop;
    h.scrollLeft = el.scrollLeft;
}

function checkTab(el, e) {
    if (e.key === "Tab") {
        e.preventDefault();
        const [s, epos] = [el.selectionStart, el.selectionEnd];
        el.value = el.value.slice(0, s) + "\t" + el.value.slice(epos);
        el.selectionStart = el.selectionEnd = s + 1;
        update(el.value);
        adjustEditorHeight();
    }
}

async function loadConfig() {
    const loader = document.getElementById("loader");
    const editor = document.getElementById("editor");
    loader.style.display = "block";
    editor.style.visibility = "hidden";

    const res = await fetch('/config/sensors');
    const data = await res.json();
    const text = JSON.stringify(data, null, 2);
    editor.value = text;
    update(text);
    adjustEditorHeight();

    loader.style.display = "none";
    editor.style.visibility = "visible";
}

async function saveConfig() {
    const raw = document.getElementById("editor").value;
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

function adjustEditorHeight() {
    const editor = document.getElementById('editor');
    const wrapper = document.getElementById('highlighting-wrapper');
    editor.style.height = 'auto'; // Reset
    const newHeight = editor.scrollHeight;
    editor.style.height = newHeight + 'px';
    wrapper.style.height = newHeight + 'px';
}

document.getElementById("editor").addEventListener("input", adjustEditorHeight);

loadConfig();
