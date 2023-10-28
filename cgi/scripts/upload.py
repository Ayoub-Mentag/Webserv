#!/usr/bin/env python
import os
# os.environ.get("name", "Unknown")
data = os.environ.get("0", "EMPTY")


print("Content-Type: text/html\r\n\r\n")
print("<html>")
print("<head><title>Form Submission</title></head>")
print("<body>")
print("<h1>Form Data Received</h1>")
f = open("demofile3.txt", "w")
f.write(data)
f.close()
# print(f"File '{file_name}' has been uploaded successfully")
print("</body>")
print("</html>")
