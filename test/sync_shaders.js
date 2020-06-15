const { readdirSync, unlinkSync } = require("fs");
const { join } = require("path");
const { exec } = require("child_process");

const SHADER_DIR = join(__dirname, "assets", "shaders");

// Clear all SPIR-V shaders.
readdirSync(SHADER_DIR)
    .filter(shader_dir_file => shader_dir_file.endsWith(".spv"))
    .map(spirv_file => join(SHADER_DIR, spirv_file))
    .forEach(spirv_file_path => unlinkSync(spirv_file_path));

// Compile new SPIR-V shaders.
readdirSync(SHADER_DIR)
    .map((shader) => ({ shader: join(SHADER_DIR, shader), output: join(SHADER_DIR, `${shader}.spv`) }))
    .forEach((cmd_data) => {
        const CMD = "C:\\Users\\joela\\dev\\libs\\VulkanSDK\\1.1.130.0\\Bin32\\glslc.exe";
        let cmd = `"${CMD}" "${cmd_data.shader}" -o "${cmd_data.output}"`;

        exec(cmd, (err, stdout, stderr) => {
            console.log(cmd);

            if(err) {
                console.error(`\x1b[31m${err.message}\x1b[0m`);
            }
        });
    });
