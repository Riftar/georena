"use strict";

const {execFileSync} = require("node:child_process");
const [, , validatorPath, generator] = process.argv;
const validator = require(validatorPath);

async function main() {
  const seeds = [...Array.from({length: 254}, (_, seed) => seed), 2147483648, 4294967295];
  const cases = ["race", "fps"].flatMap(mode => seeds.map(seed => [mode, String(seed)]));
  for (const car of ["coupe", "rally"]) {
    for (let level = 1; level <= 5; level++) cases.push(["car", car, String(level)]);
  }

  let errors = 0;
  let warnings = 0;
  for (const args of cases) {
    const uri = args.join("-") + ".glb";
    const buffer = execFileSync(generator, [...args, "-"]);
    const report = await validator.validateBytes(new Uint8Array(buffer), {format: "glb", uri});
    errors += report.issues.numErrors;
    warnings += report.issues.numWarnings;
    for (const issue of report.issues.messages.filter(message => message.severity === 0)) {
      console.error(`${uri} ${issue.code} ${issue.pointer ?? ""} ${issue.message}`);
    }
  }
  console.log(`Khronos glTF Validator ${validator.version()}: ${cases.length} GLBs, ${errors} errors, ${warnings} warnings.`);
  if (errors) process.exitCode = 1;
}

main().catch(error => {
  console.error(error.message);
  process.exitCode = 1;
});
