dummy:
	$(info Available targets: "format")

format:
	clang-format -i -style=file */*/*.c
	clang-format -i -style=file */*/*.h