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

    loader.style.display = "none";
    editor.style.visibility = "visible";
}

function saveConfig() {
    // Show confirmation modal
    document.getElementById("confirm-modal").classList.remove("hidden");
}

function showMessage(text, type) {
    const container = document.getElementById("message-box-container");
    const box = document.getElementById("message-box");

    box.className = ""; // reset
    box.classList.add(type === "success" ? "success" : "error");
    box.textContent = text;

    container.className = "visible";

    setTimeout(() => {
        container.classList.remove("visible");
    }, 5000);
}

async function confirmSave(doSave) {
    const modal = document.getElementById("confirm-modal");
    modal.classList.add("hidden");

    if (!doSave) return;

    const raw = document.getElementById("editor").value;
    try {
        JSON.parse(raw);
    } catch (e) {
        showMessage("Invalid JSON", "error");
        return;
    }

    try {
        const result = await fetch("/config/sensors", {
            method: "POST",
            headers: {
                'Accept': 'text/plain',
                'Content-Type': 'application/json'
            },
            body: raw
        });

        if (result.ok) {
            showMessage("Saved!", "success");
        } else {
            const error_message = await result.text();
            showMessage(error_message, "error");
        }
    } catch (err) {
        showMessage("Network error", "error");
    }
}

loadConfig();
