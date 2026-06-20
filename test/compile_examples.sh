#!/bin/bash

# AnalogButtons Library Compilation Test Script
# Tests compilation across multiple Arduino platforms and all examples

# Not using set -e: compilation failures are tracked via counters; aborting early
# would skip remaining examples and suppress the summary.

#######################################################################
# CONFIGURATION CONSTANTS
#######################################################################

# Test platforms configuration
# Format: "FQBN|Display Name|PIO Board ID"
declare -a PLATFORMS=(
    "esp8266:esp8266:d1_mini|Wemos D1 Mini (ESP8266)|d1_mini"
    # esp32dev defines A0=GPIO36; M5Stack Core2 uses G0/G1 aliases instead.
    "esp32:esp32:esp32dev|ESP32 Dev Module (ESP32)|esp32dev"
    "arduino:avr:nano|Arduino Nano (AVR)|nanoatmega328"
)

# Examples directory
EXAMPLES_DIR="../examples"

#######################################################################
# DISPLAY CONFIGURATION
#######################################################################

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

EXAMPLES=($(find $EXAMPLES_DIR -name "*.ino" -exec dirname {} \; | sort | uniq))

print_status() {
    local status=$1
    local message=$2
    case $status in
        "INFO")    echo -e "${BLUE}[INFO]${NC} $message" ;;
        "SUCCESS") echo -e "${GREEN}[PASS]${NC} $message" ;;
        "ERROR")   echo -e "${RED}[FAIL]${NC} $message" ;;
        "WARNING") echo -e "${YELLOW}[WARN]${NC} $message" ;;
    esac
}

#######################################################################
# COMPILATION
#######################################################################

test_compilation() {
    local platform_fqbn=$1
    local platform_name=$2
    local pio_env=$3
    local example_path=$4
    local use_tool=$5
    local example_name=$(basename "$example_path")
    local ino_file="$example_path/$example_name.ino"

    if [ ! -f "$ino_file" ]; then
        echo -e "  $example_name ${YELLOW}[SKIP]${NC} (file not found)"
        return 0
    fi

    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -n "  $example_name "

    if [[ "$use_tool" == "pio" ]]; then
        local platform_pkg=""
        case "$pio_env" in
            d1_mini)        platform_pkg="espressif8266" ;;
            # Pin espressif32 to 6.x (Arduino ESP32 2.x / IDF 4.x) to avoid the
            # y0() Bessel-function name clash introduced in Arduino ESP32 3.x.
            esp32dev)       platform_pkg="espressif32@^6.9.0" ;;
            nanoatmega328)  platform_pkg="atmelavr" ;;
        esac

        # Build a proper PlatformIO project in a temp dir so that pio run
        # (not pio ci) handles .ino → Arduino preprocessing correctly.
        # pio ci with --project-conf skips the Arduino framework includes on AVR,
        # causing symbols like A0 to be undefined.
        local lib_root
        lib_root=$(cd "$example_path/../.." && pwd)

        local tmp_proj
        tmp_proj=$(mktemp -d)
        local tmp_out
        tmp_out=$(mktemp)

        mkdir -p "$tmp_proj/src"
        cp "$ino_file" "$tmp_proj/src/"

        cat > "$tmp_proj/platformio.ini" << EOF
[env:test]
platform = $platform_pkg
board = $pio_env
framework = arduino
lib_extra_dirs = $lib_root
lib_deps =
    https://github.com/LennartHennigs/Button2.git
EOF

        if (cd "$tmp_proj" && pio run -e test > "$tmp_out" 2>&1); then
            echo -e "${GREEN}[PASS]${NC}"
            PASSED_TESTS=$((PASSED_TESTS + 1))
            rm -rf "$tmp_proj" "$tmp_out"
            return 0
        else
            echo -e "${RED}[FAIL]${NC}"
            FAILED_TESTS=$((FAILED_TESTS + 1))
            echo -e "    ${RED}Error details:${NC}"
            tail -15 "$tmp_out" | sed 's/^/     /'
            echo ""
            rm -rf "$tmp_proj" "$tmp_out"
            return 1
        fi
    else
        local tmp_out
        tmp_out=$(mktemp)
        if arduino-cli compile --fqbn "$platform_fqbn" "$ino_file" --output-dir "/tmp/arduino-build-$example_name-$(date +%s)" > "$tmp_out" 2>&1; then
            echo -e "${GREEN}[PASS]${NC}"
            PASSED_TESTS=$((PASSED_TESTS + 1))
            rm -f "$tmp_out"
            return 0
        else
            echo -e "${RED}[FAIL]${NC}"
            FAILED_TESTS=$((FAILED_TESTS + 1))
            echo -e "    ${RED}Error details:${NC}"
            tail -15 "$tmp_out" | sed 's/^/     /'
            echo ""
            rm -f "$tmp_out"
            return 1
        fi
    fi
}

#######################################################################
# PREREQUISITES
#######################################################################

check_prerequisites() {
    local use_tool=$1
    print_status "INFO" "Checking prerequisites..."

    if [[ "$use_tool" == "pio" ]]; then
        if ! command -v pio &> /dev/null; then
            print_status "ERROR" "PlatformIO (pio) is not installed or not in PATH"
            echo "Please install PlatformIO: https://platformio.org/install/cli"
            exit 1
        fi
        print_status "SUCCESS" "PlatformIO found: $(pio --version | head -1)"

        # esptool requires the 'intelhex' pip package but PlatformIO's bundled
        # esptool doesn't always install it. Find pip in PlatformIO's venv and
        # ensure intelhex is present (pip install is a no-op if already installed).
        local pio_pip=""
        for _p in "$HOME/.platformio/penv/bin/pip3" "$HOME/.platformio/penv/bin/pip"; do
            [[ -x "$_p" ]] && { pio_pip="$_p"; break; }
        done
        if [[ -n "$pio_pip" ]]; then
            if ! "$pio_pip" show intelhex &>/dev/null; then
                print_status "WARNING" "Installing missing 'intelhex' into PlatformIO venv..."
                "$pio_pip" install intelhex -q \
                    && print_status "SUCCESS" "intelhex installed" \
                    || print_status "WARNING" "Could not install intelhex; ESP builds may fail"
            fi
        fi
    else
        if ! command -v arduino-cli &> /dev/null; then
            print_status "ERROR" "arduino-cli is not installed or not in PATH"
            echo "Please install arduino-cli: https://arduino.github.io/arduino-cli/"
            exit 1
        fi
        print_status "SUCCESS" "arduino-cli found: $(arduino-cli version | head -1)"
    fi
    echo ""
}

check_cores() {
    local use_tool=$1
    if [[ "$use_tool" != "pio" ]]; then
        print_status "INFO" "Checking required Arduino cores..."
        local cores_output=$(arduino-cli core list)
        local missing_cores=()
        echo "$cores_output" | grep -q "esp8266:esp8266" || missing_cores+=("esp8266:esp8266")
        echo "$cores_output" | grep -q "esp32:esp32"     || missing_cores+=("esp32:esp32")
        echo "$cores_output" | grep -q "arduino:avr"     || missing_cores+=("arduino:avr")

        if [ ${#missing_cores[@]} -gt 0 ]; then
            print_status "WARNING" "Missing cores: ${missing_cores[*]}"
            print_status "INFO" "Install with:"
            for core in "${missing_cores[@]}"; do echo "  arduino-cli core install $core"; done
            echo ""
        else
            print_status "SUCCESS" "All required cores are installed"
            echo ""
        fi
    fi
}

#######################################################################
# RUN
#######################################################################

run_all_tests() {
    local selected_platform="$1"
    local use_tool="$2"

    print_status "INFO" "Starting compilation tests for AnalogButtons library"
    echo "======================================================="
    echo ""

    for platform_info in "${PLATFORMS[@]}"; do
        IFS='|' read -r platform_fqbn platform_name pio_env <<< "$platform_info"

        if [[ -n "$selected_platform" ]]; then
            local board_name=$(echo "$platform_fqbn" | rev | cut -d':' -f1 | rev)
            if [[ "$selected_platform" != "$board_name" ]] &&
               [[ "$platform_name" != *"$selected_platform"* ]] &&
               [[ "$platform_fqbn" != *"$selected_platform"* ]] &&
               [[ "$pio_env" != *"$selected_platform"* ]]; then
                continue
            fi
        fi

        print_status "INFO" "Testing platform: $platform_name (using $use_tool)"
        echo "----------------------------------------"

        for example_path in "${EXAMPLES[@]}"; do
            test_compilation "$platform_fqbn" "$platform_name" "$pio_env" "$example_path" "$use_tool"
        done
        echo ""
    done
}

#######################################################################
# SUMMARY
#######################################################################

show_summary() {
    local platform="$1"
    echo ""
    echo "======================================================="
    print_status "INFO" "Compilation Test Summary"
    echo "======================================================="
    [[ -n "$platform" ]] && echo "Platform:     $platform" || echo "Platform:     All platforms"
    echo "Total Tests:  $TOTAL_TESTS"
    echo -e "Passed:       ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed:       ${RED}$FAILED_TESTS${NC}"

    if [ $TOTAL_TESTS -eq 0 ]; then
        echo ""
        print_status "WARNING" "No tests were executed!"
        [[ -n "$platform" ]] && echo "The specified platform '$platform' was not found."
        exit 1
    elif [ $FAILED_TESTS -eq 0 ]; then
        echo ""
        print_status "SUCCESS" "All compilation tests passed!"
    else
        echo ""
        print_status "ERROR" "Some compilation tests failed!"
        exit 1
    fi
}

#######################################################################
# HELP
#######################################################################

show_help() {
    echo "AnalogButtons Library Compilation Test Script"
    echo ""
    echo "Usage: $0 [OPTIONS] [PLATFORM]"
    echo ""
    echo "Options:"
    echo "  -h, --help           Show this help message"
    echo "  --tool=TOOL          Use 'pio' (default) or 'arduino-cli'"
    echo ""
    echo "Platform (optional, filters to one platform):"
    echo "  - Board name:    d1_mini, esp32dev, nano"
    echo "  - Display name:  Wemos, ESP32, Nano"
    echo "  - FQBN fragment: esp8266, esp32, avr"
    echo ""
    echo "Examples:"
    echo "  $0                          # All platforms via PlatformIO"
    echo "  $0 --tool=arduino-cli       # All platforms via arduino-cli"
    echo "  $0 esp8266                  # ESP8266 only"
    echo "  $0 --tool=arduino-cli avr   # AVR only via arduino-cli"
    echo ""
}

#######################################################################
# MAIN
#######################################################################

main() {
    local platform=""
    local use_tool="pio"

    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help) show_help; exit 0 ;;
            --tool=*)
                use_tool="${1#*=}"
                if [[ "$use_tool" != "pio" && "$use_tool" != "arduino-cli" ]]; then
                    print_status "ERROR" "Invalid tool: $use_tool. Must be 'pio' or 'arduino-cli'"
                    exit 1
                fi
                shift ;;
            -*)
                print_status "ERROR" "Unknown option: $1"
                show_help; exit 1 ;;
            *)
                if [[ -z "$platform" ]]; then platform="$1"
                else print_status "ERROR" "Multiple platforms specified"; show_help; exit 1; fi
                shift ;;
        esac
    done

    cd "$(dirname "$0")"

    print_status "INFO" "Using build tool: $use_tool"
    [[ -n "$platform" ]] && print_status "INFO" "Testing selected platform: $platform"
    echo ""

    check_prerequisites "$use_tool"
    check_cores "$use_tool"
    run_all_tests "$platform" "$use_tool"
    show_summary "$platform"
}

main "$@"
