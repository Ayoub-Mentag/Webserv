import cgi

print("------------")
form = cgi.FieldStorage()
input_string = form.getvalue("photo")  # Access form field named "photo"
print(input_string)
print("------------")
print("Content-Type: text/html\r\n\r\n")
print("<html>")
print("<head><title>Form Submission</title></head>")
print("<body>")
print("<h1>Form Data Received</h1>")


# print(input_string)
print("</body>")
print("</html>")
