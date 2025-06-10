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

    const res = await fetch('/api/sensors/config');
    const data = await res.json();
    const text = JSON.stringify(data, null, 2);
    editor.value = text;
    update(text);

    loader.style.display = "none";
    editor.style.visibility = "visible";
}

function showMessage(text, type) {
    const container = document.getElementById("message-box-container");
    const box = document.getElementById("message-box");

    box.className = "";
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
        const result = await fetch("/api/sensors/config", {
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

const overlayMenu = document.getElementById("overlay-menu");
const menuButton = document.getElementById("menu-button");

menuButton.addEventListener("click", () => {
    overlayMenu.classList.remove("hidden");
});

overlayMenu.addEventListener("click", (e) => {
    if (e.target === overlayMenu)
        overlayMenu.classList.add("hidden");
});

function menuSaveConfig() {
    overlayMenu.classList.add("hidden");
    document.getElementById("confirm-modal").classList.remove("hidden");
}

function menuDownloadConfig() {
    overlayMenu.classList.add("hidden");

    const raw = document.getElementById("editor").value;
    const blob = new Blob([raw], { type: "application/json" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = "sensors_config_" + new Date(Date.now()).toISOString().replaceAll(":", "_").replaceAll(".", "_").replaceAll("-", "_") + ".json";
    a.click();
    URL.revokeObjectURL(url);
}

function menuUploadConfig() {
    overlayMenu.classList.add("hidden");

    const input = document.getElementById("file-input");
    input.value = "";
    input.click();

    input.onchange = async (e) => {
        const file = e.target.files[0];
        if (!file) return;

        const text = await file.text();
        try {
            uploadedFileContent = JSON.parse(text);
            const editor = document.getElementById("editor");
            editor.value = JSON.stringify(uploadedFileContent, null, 2);
            update(editor.value);
        } catch (err) {
            alert("Invalid JSON file.");
        }
    };
}

loadConfig();
