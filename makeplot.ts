#!/usr/bin/env bun
//@ts-nocheck

const foo = Bun.file("runs/annealing-0.txt");

const extractCost = (text : string) => {
  const idx1 = text.indexOf("Solution cost: ");
  if(idx1 !== -1) {
    return Number(text.substring(idx1 + "Solution cost: ".length).match(/\d+/)[0]);
  }

}




const plotToFile = async (file : string, testbed: string[], algs : string[]) => {


  let r = `${algs.length},` + algs.join(",") + "\n";
  for(let i = 0; i < testbed.length; i++) {
    r += testbed[i] + ","
    for(let k = 0; k < algs.length; k++){
      const numb = extractCost(await Bun.file(`./runs/${algs[k]}-${i}.txt`).text());
      r += String(numb)
      if(k < algs.length - 1) {
        r += ","
      }
    }
    r += "\n"
  }

  await Bun.write(file, r);
}


const testbed = [
  'pr1002',
  'a280',
  'att48',
  'att532',
  'bier127',
  'eil101',
  'kroA150',
  'kroA200',
  'pr299',
  'pr439',
]

const algs1 = [
  //'grasp',
  'vns',
  'tabustep',
  'tabulinear',
  'taburandom',
  'annealing',
  'genetic',
]

const algs2 = [
  'hardfixing',
  'localbranch',
]

for(const [name, algs] of [
  ['metaheur', algs1],
  ['mateheur', algs2],
  ['totalheur', [...algs1, ...algs2]],

]){
  await plotToFile(`plot/perf/${name}.csv`, testbed, algs);
  Bun.spawn(['python3', 'plot/perfprof.py', '-D', ',', `plot/perf/${name}.csv`, `imgs/${name}.png`, '-P', " ", '--x-label', 'Cost ratio', '-M', '-1'])


}