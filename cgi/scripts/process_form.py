#!/usr/bin/env python
import os

name = os.environ.get("name", "Unknown")

# Process the data (e.g., you can save it to a file or a database)

# Generate a response
print("Content-Type: text/html\r\n\r\n")
print("<html>")
print("<head><title>Form Submission</title></head>")
print("<body>")
print("<h1>Form Data Received</h1>")
print(f"<p><strong>Name:</strong> {name}</p>")
print("</body>")
print("</html>")