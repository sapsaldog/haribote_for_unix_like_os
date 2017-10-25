MKDIR_P := mkdir -p
RM_RF := rm -rf
OUT_DIR := build

directories: $(OUT_DIR)

$(OUT_DIR):
	${MKDIR_P} $(OUT_DIR)

$(OUT_DIR)/makefont: |directories
	g++ makefont/makefont.cpp -o $@

makefont : $(OUT_DIR)/makefont

clean:
	${RM_RF} $(OUT_DIR)
