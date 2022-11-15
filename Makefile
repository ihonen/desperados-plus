BUILD_DIR := .build
PKG_DIR   := $(BUILD_DIR)/package

# ------------------------------------------------------------------------------

configure-debug:
	cmake -B $(BUILD_DIR) -S . -A Win32 -DDP_DEV_BUILD=ON
.PHONY: configure-debug

configure-release:
	cmake -B $(BUILD_DIR) -S . -A Win32
.PHONY: configure-release

build-debug: configure-debug
	cmake --build $(BUILD_DIR) --config Debug --parallel
.PHONY: build-debug

build-release: configure-release
	cmake --build $(BUILD_DIR) --config Release --parallel
.PHONY: build-release

# ------------------------------------------------------------------------------

package: build-release
	rm -rf $(BUILD_DIR)/package/Desperados+
	cmake --install $(BUILD_DIR) --verbose --prefix $(PKG_DIR)/Desperados+ --config Release
	cd $(PKG_DIR)/Desperados+ && 7z a ../Desperados+.zip *
.PHONY: package

# ------------------------------------------------------------------------------

clean:
	rm -rf $(BUILD_DIR)
.PHONY: clean
