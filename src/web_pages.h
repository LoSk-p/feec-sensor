#include <Arduino.h>

String generateHtmlContent(const char* robonomicsSs58Address) {
    return String(R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>WiFi Credentials</title>
    <style>
        body {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh; /* Full viewport height */
            font-family: 'Helvetica Neue', Arial, sans-serif;
            background-color: #e9ecef; /* Light gray background */
            margin: 0;
        }
        .container {
            background-color: white; /* White background for form */
            padding: 40px; /* Increased padding */
            border-radius: 8px;
            box-shadow: 0 4px 20px rgba(0, 0, 0, 0.1);
            width: 600px; /* Increased width for the form */
            text-align: center; /* Center align text */
        }
        h1 {
            color: #333; /* Dark color for headings */
            margin-bottom: 20px;
        }
        .ss58-label {
            font-size: 24px; /* Increased font size for the label */
            color: #333; /* Same color as h1 */
            margin-top: 20px;
        }
        .ss58-address {
            color: #555; /* Gray color for the SS58 address */
            font-size: 16px; /* Keep the original size for the address */
        }
        input[type="text"], input[type="password"] {
            width: calc(100% - 20px); /* Adjust width for padding */
            padding: 12px; /* Increased padding for inputs */
            margin: 10px 0;
            border: 1px solid #ccc;
            border-radius: 4px;
            font-size: 16px; /* Larger font size */
        }
        input[type="submit"] {
            background-color: #007bff; /* Bootstrap primary color */
            color: white; /* White text */
            padding: 12px; /* Increased padding for button */
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 16px; /* Larger font size */
        }
        input[type="submit"]:hover {
            background-color: #0056b3; /* Darker blue on hover */
        }
        .toggle-password {
            position: relative;
            display: inline-block;
        }
        .toggle-password button {
            position: absolute;
            right: 10px;
            top: 50%;
            transform: translateY(-50%);
            border: none;
            background: none;
            cursor: pointer;
            font-size: 14px;
        }
    </style>
    <script>
        function togglePassword() {
            var passwordField = document.getElementById("password");
            var toggleButton = document.getElementById("toggleButton");
            if (passwordField.type === "password") {
                passwordField.type = "text";
                toggleButton.textContent = "Hide";
            } else {
                passwordField.type = "password";
                toggleButton.textContent = "Show";
            }
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Enter WiFi Credentials</h1>
        <form action="/save" method="POST">
            SSID: <input type="text" name="ssid" required><br>
            <div class="toggle-password">
                Password: <input type="password" id="password" name="password" required>
                <button type="button" id="toggleButton" onclick="togglePassword()">Show</button>
            </div>
            <br>
            Server IP Address (Optional): <input type="text" name="server_ip"><br>
            <input type="submit" value="Save">
        </form>
        <h1>Robonomics SS58 Address</h1>
        <div class="ss58-address"><strong>)rawliteral" + String(robonomicsSs58Address) + R"rawliteral(</strong></div>
    </div>
</body>
</html>
)rawliteral");
}