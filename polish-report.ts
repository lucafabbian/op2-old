#!/usr/bin/env bun
//@ts-nocheck

let text = await Bun.file("TSP.md").text();

text = `---
header-includes: |
  \\usepackage{float}
  \\makeatletter
  \\def\\fps@figure{H}
  \\makeatother
---


` + text.replaceAll("![png]", "![]")

Bun.write("TSP.md", text)