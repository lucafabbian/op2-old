import os

if __name__ == '__main__':
  with(open(os.path.join(os.path.dirname(__file__), 'report', 'TSP.tex'))) as f:
    s = f.read()
    s = s.replace('CTC', '\\cite{')
    s = s.replace('TCT', '}')

  with open(os.path.join(os.path.dirname(__file__), 'report', 'TSP.tex'), 'w') as file:
    # Write the string to the file
    file.write(s)
