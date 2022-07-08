set_languages("cxx17")
add_rules("mode.debug", "mode.release")

local libs = { "fmt", "glad", "glfw", "glm", "imguizmo" }

add_includedirs("include")
add_requires(table.unpack(libs))
add_requireconfs(
	"imguizmo.imgui",
	{ override = true, version = "v1.83-docking", configs = { glfw_opengl3 = true, useglad = true } }
)

add_requires(
	"imgui",
	{ alias = "imgui-docking", version = "v1.83-docking", configs = { glfw_opengl3 = true, useglad = true } }
)

table.insert(libs, "imgui-docking")

target("bloom_lib")
  set_kind("static")
  add_files("source/**/*.cpp")
  add_packages(table.unpack(libs))

target("BloomCG")
  set_kind("binary")
  add_files("standalone/main.cpp")
  add_packages(table.unpack(libs))
  add_deps("bloom_lib")
