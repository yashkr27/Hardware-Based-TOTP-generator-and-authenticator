 const { app, BrowserWindow, ipcMain } = require('electron');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

let port;
let latestOtp = '';
let parser;

app.whenReady().then(() => {
  const win = new BrowserWindow({
    width: 400,
    height: 300,
    webPreferences: { nodeIntegration: true, contextIsolation: false },
  });

  win.loadFile('index.html');

  /* ▶️ 1.  Open the Arduino’s COM port */
  port   = new SerialPort({ path: 'COM3', baudRate: 9600 }); // ← put your real port
  parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }));

  /* ▶️ 2.  Handle every line coming from Arduino */
  parser.on('data', (line) => {
    const text = line.trim();

    /* Forward **only** 6‑digit lines to GUI */
    if (/^\d{6}$/.test(text)) {
      latestOtp = text;
      win.webContents.send('otp-update', latestOtp);
      console.log('[MAIN] OTP from Arduino:', latestOtp);
    } else {
      console.log('[MAIN] Non‑OTP line:', text);
    }
  });

  port.on('error', (err) => {
    console.error('Serial Port Error:', err.message);
  });

  /* ▶️ 3.  When the user clicks “Google” or “GitHub” */
  ipcMain.on('send-service', (_evt, serviceName) => {
    if (port?.isOpen) port.write(serviceName + '\n');

    /* Wait for the **next** 6‑digit OTP, ignore any other lines */
    const waitForOtp = (line) => {
      const text = line.trim();
      if (!/^\d{6}$/.test(text)) return;        // skip non‑OTP lines

      const otpForLogin = text;                 // freeze the fresh OTP
      console.log('[MAIN] OTP for login:', otpForLogin);

      const loginWindow = new BrowserWindow({
        width: 400,
        height: 250,
        webPreferences: { nodeIntegration: true, contextIsolation: false },
      });

      loginWindow.loadFile('login.html');
      loginWindow.webContents.on('did-finish-load', () => {
        loginWindow.webContents.send('otp-latest', otpForLogin);
      });

      parser.off('data', waitForOtp);           // listen just once
    };

    parser.on('data', waitForOtp);              // temporary listener
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});
