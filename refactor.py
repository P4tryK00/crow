import os, re

dir_path = r"D:\Programowanie\crow\templates"
for filename in os.listdir(dir_path):
    if filename.endswith(".html"):
        filepath = os.path.join(dir_path, filename)
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
            
        content = re.sub(r'<style>[\s\S]*?</style>', '<link rel="stylesheet" href="/static/style.css">', content)
        # remove style block from body
        content = re.sub(r'<body([^>]*) style="font-family: \'Inter\', sans-serif;"([^>]*)>', r'<body\1\2>', content)
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
