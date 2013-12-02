for %%F in (*.md) do pandoc.exe --standalone --from markdown_github --to html5 --css github.css --output %%~nF.html %%F
