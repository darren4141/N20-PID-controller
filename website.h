const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Motor Control</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
    input[type=range] { width: 80%; }
    .value { font-size: 20px; color: #333; }
    button { margin-top: 20px; padding: 10px; font-size: 16px; margin-right: 10px; }
    table { width: 80%; margin: 20px auto; border-collapse: collapse; }
    th, td { border: 1px solid #000; padding: 8px; text-align: center; }
    th { background-color: #ddd; }
  </style>
</head>
<body>
  <h2>Motor Control</h2>
  <p>Adjust power:</p>
  <input type="range" id="slider" min="0" max="255" value="0" oninput="updateSlider(this.value)">
  <p>Power: <span id="sliderValue">0</span></p>
  <h3>Motor Speeds</h3>
  <p>Motor 1: <span id="motor1Speed">0</span> RPM</p>
  <p>Motor 2: <span id="motor2Speed">0</span> RPM</p>

  <button onclick="logValues()">Log Values</button>
  <button onclick="downloadCSV()">Download CSV</button>
  <button onclick="clearLog()">Clear Log</button>

  <h3>Logged Values</h3>
  <table id="logTable">
    <tr>
      <th>Timestamp</th>
      <th>Power</th>
      <th>Motor 1 Speed</th>
      <th>Motor 2 Speed</th>
    </tr>
  </table>

  <script>
    let logData = []; // Array to store logged values

    function updateSlider(value) {
      document.getElementById('sliderValue').innerText = value;
      fetch(`/setPower?value=${value}`); // Send power value to ESP32
    }

    function fetchSpeeds(callback) {
      fetch('/getSpeeds') // Request motor speeds from ESP32
        .then(response => response.json())
        .then(data => {
          document.getElementById('motor1Speed').innerText = data.motor1;
          document.getElementById('motor2Speed').innerText = data.motor2;

          if (callback) callback(data); // Call the callback function if provided
        })
        .catch(error => console.error("Failed to fetch speeds:", error));
    }

    function logValues() {
      fetchSpeeds((data) => {
        let timestamp = new Date().toLocaleTimeString();
        let power = document.getElementById('sliderValue').innerText;
        logData.push([timestamp, power, data.motor1, data.motor2]);

        // Add new row to the table
        let table = document.getElementById('logTable');
        let newRow = table.insertRow();
        newRow.insertCell(0).innerText = timestamp;
        newRow.insertCell(1).innerText = power;
        newRow.insertCell(2).innerText = data.motor1;
        newRow.insertCell(3).innerText = data.motor2;

        console.log("Logged:", timestamp, power, data.motor1, data.motor2);
      });
    }

    function downloadCSV() {
      let csvContent = "data:text/csv;charset=utf-8,Timestamp,Power,Motor1 Speed,Motor2 Speed\n";
      logData.forEach(row => {
        csvContent += row.join(",") + "\n";
      });

      let encodedUri = encodeURI(csvContent);
      let link = document.createElement("a");
      link.setAttribute("href", encodedUri);
      link.setAttribute("download", "motor_log.csv");
      document.body.appendChild(link);
      link.click();
      document.body.removeChild(link);
    }

    function clearLog() {
      logData = []; // Clear the stored log data
      let table = document.getElementById('logTable');

      // Remove all rows except the header
      while (table.rows.length > 1) {
        table.deleteRow(1);
      }
    }
  </script>
</body>
</html>
)rawliteral";