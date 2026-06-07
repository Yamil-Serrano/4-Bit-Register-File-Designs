#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Exhaustive write/read test on all 4 registers (0-15 each)
// Follow your Reg File design and pinout to update these arrays if needed 
// Current pin setup assumes 4-Bit Register File RB1534B Rev 2.0

// Data Input [LSB -> MSB]
const int PIN_DATAIN[] = {15, 2, 4, 16};

// Write Register Select (RD) [LSB -> MSB]
const int PIN_RD[] = {17, 5};

// Read Register Select (RR1) [LSB -> MSB]
const int PIN_RR1[] = {18, 19};

// Control Signals
const int PIN_CLOCK    = 22;
const int PIN_REGWRITE = 23;

// Register Output RS1 [LSB -> MSB]
const int PIN_RS1[] = {14, 27, 26, 25};

// LCD I2C pins
const int I2C_SDA = 33;
const int I2C_SCL = 32;

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define ANSI_GREEN  "\033[32m"
#define ANSI_RED    "\033[31m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_CYAN   "\033[36m"
#define ANSI_RESET  "\033[0m"

void printDivider() {
    Serial.println("------------------------------------------------------------");
}

void setDataIn(byte value) {
    for (int i = 0; i < 4; i++)
        digitalWrite(PIN_DATAIN[i], bitRead(value, i));
}

void setRD(byte reg) {
    for (int i = 0; i < 2; i++)
        digitalWrite(PIN_RD[i], bitRead(reg, i));
}

void setRR1(byte reg) {
    for (int i = 0; i < 2; i++)
        digitalWrite(PIN_RR1[i], bitRead(reg, i));
}

void pulseClock() {
    digitalWrite(PIN_CLOCK, LOW);
    delayMicroseconds(5);
    digitalWrite(PIN_CLOCK, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_CLOCK, LOW);
    delayMicroseconds(5);
}

byte readRS1() {
    byte value = 0;
    for (int i = 0; i < 4; i++)
        if (digitalRead(PIN_RS1[i]))
            value |= (1 << i);
    return value;
}

void writeRegister(byte reg, byte value) {
    setDataIn(value);
    setRD(reg);
    digitalWrite(PIN_REGWRITE, HIGH);
    delayMicroseconds(5);
    pulseClock();
    delayMicroseconds(10);
    digitalWrite(PIN_REGWRITE, LOW);
}

byte readRegister(byte reg) {
    setRR1(reg);
    delayMicroseconds(10);
    return readRS1();
}

void printBin4(byte val) {
    Serial.print("0b");
    for (int i = 3; i >= 0; i--)
        Serial.print((val >> i) & 1);
}

//  Unique Pattern Test — isolates read-path decoder
bool runUniquePatternTest() {
    Serial.println();
    printDivider();
    Serial.println("                  UNIQUE PATTERN TEST");
    printDivider();
    Serial.println("  Reg  |  Written   |  Read      |  Result");
    printDivider();

    const byte patterns[4] = { 0x1, 0x2, 0x4, 0x8 };

    for (byte reg = 0; reg < 4; reg++)
        writeRegister(reg, patterns[reg]);

    bool allPass = true;

    for (byte reg = 0; reg < 4; reg++) {
        byte observed = readRegister(reg);
        bool ok = (observed == patterns[reg]);
        if (!ok) allPass = false;

        Serial.print("  R"); Serial.print(reg);
        Serial.print("   |  ");
        printBin4(patterns[reg]);
        Serial.print("  |  ");
        printBin4(observed);
        Serial.print("  |  ");
        if (ok) {
            Serial.print(ANSI_GREEN); Serial.println("PASS");
        } else {
            Serial.print(ANSI_RED); Serial.println("FAIL");
        }
        Serial.print(ANSI_RESET);
    }
    printDivider();
    return allPass;
}

//  Exhaustive Test — all 16 values per register
struct RegResult {
    int hits;
    int total;
    float pct;
};

RegResult regResults[4];

void runExhaustiveTest() {
    Serial.println();
    printDivider();
    Serial.println("                 EXHAUSTIVE REGISTER TEST");
    printDivider();

    int totalFails = 0;

    for (byte reg = 0; reg < 4; reg++) {
        int hits = 0;

        Serial.println();
        Serial.print("  [R"); Serial.print(reg); Serial.println("]");
        Serial.println("  Write      | Read       | Result");
        Serial.println("  --------------------------------");

        for (byte val = 0; val < 16; val++) {
            writeRegister(reg, val);
            byte observed = readRegister(reg);
            bool ok = (observed == val);
            if (ok) hits++;
            else totalFails++;

            Serial.print("  "); printBin4(val);
            Serial.print(" ("); Serial.print(val); Serial.print(")  | ");
            printBin4(observed);
            Serial.print(" ("); Serial.print(observed); Serial.print(")  | ");
            if (ok) {
                Serial.print(ANSI_GREEN); Serial.println("HIT");
            } else {
                Serial.print(ANSI_RED); Serial.println("MISS");
            }
            Serial.print(ANSI_RESET);
        }

        regResults[reg] = { hits, 16, (hits * 100.0f) / 16 };

        Serial.print("  Result: ");
        if (regResults[reg].pct == 100.0f) Serial.print(ANSI_GREEN);
        else Serial.print(ANSI_YELLOW);
        Serial.print(hits); Serial.print("/16 (");
        Serial.print(regResults[reg].pct, 1); Serial.println("%)");
        Serial.print(ANSI_RESET);
    }

    // Summary table
    Serial.println();
    printDivider();
    Serial.println("                  REGISTER FILE SUMMARY");
    printDivider();
    Serial.println("  Reg  |  RD bits  |  Hits   |  Pass Rate");
    printDivider();

    for (int r = 0; r < 4; r++) {
        int b1 = (r >> 1) & 1, b0 = r & 1;
        Serial.print("  R"); Serial.print(r);
        Serial.print("   |   "); Serial.print(b1); Serial.print(" "); Serial.print(b0);
        Serial.print("     |  ");
        Serial.print(regResults[r].hits); Serial.print("/16  |  ");
        if (regResults[r].pct == 100.0f) Serial.print(ANSI_GREEN);
        else Serial.print(ANSI_YELLOW);
        Serial.print(regResults[r].pct, 1); Serial.println("%");
        Serial.print(ANSI_RESET);
    }
    printDivider();

    Serial.println("\n  [PASS] 100% match:");
    for (int r = 0; r < 4; r++)
        if (regResults[r].pct == 100.0f) {
            Serial.print("    R"); Serial.print(r);
            Serial.print(" -> "); Serial.print(ANSI_GREEN);
            Serial.println("PASS"); Serial.print(ANSI_RESET);
        }

    bool anyWarn = false;
    for (int r = 0; r < 4; r++)
        if (regResults[r].pct < 100.0f && regResults[r].pct > 0) { anyWarn = true; break; }

    Serial.print("\n  [");
    Serial.print(ANSI_YELLOW); Serial.print("WARNING"); Serial.print(ANSI_RESET);
    Serial.println("] Partial match:");
    if (!anyWarn) {
        Serial.print("    "); Serial.print(ANSI_GREEN);
        Serial.println("None"); Serial.println(ANSI_RESET);
    } else {
        for (int r = 0; r < 4; r++)
            if (regResults[r].pct < 100.0f && regResults[r].pct > 0) {
                Serial.print("    R"); Serial.print(r); Serial.print(" -> (");
                Serial.print(ANSI_YELLOW); Serial.print(regResults[r].pct, 1);
                Serial.print("%"); Serial.print(ANSI_RESET); Serial.println(")");
            }
    }

    Serial.println();
    printDivider();
    if (totalFails == 0) {
        Serial.print(ANSI_GREEN);
        Serial.println("         REGISTER FILE TEST COMPLETE — PASS");
    } else {
        Serial.print(ANSI_RED);
        Serial.print("         REGISTER FILE TEST COMPLETE — ");
        Serial.print(totalFails); Serial.println(" FAILURE(S)");
    }
    Serial.print(ANSI_RESET);
    printDivider();
}

// Function to update progress bar on LCD
void updateProgressBar(int currentStep, int totalSteps) {
    int progress = map(currentStep, 0, totalSteps, 1, 18);
    for (int i = 1; i <= 18; i++) {
        lcd.setCursor(i, 2);
        if (i <= progress) {
            lcd.print((char)255);  // Block character for progress
        } else {
            lcd.print(" ");  // Clear remaining
        }
    }
}

//  Setup
void setup() {
    delay(1000);
    Serial.begin(9600);

    // Initialize I2C with custom pins
    Wire.begin(I2C_SDA, I2C_SCL);
    
    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();
    
    // LCD Boot Screen with progress bar
    lcd.setCursor(0, 0);
    lcd.print("   Hardware Bench");
    lcd.setCursor(0, 1);
    lcd.print("   Initializing");
    lcd.setCursor(0, 3);
    lcd.print(" Made by: Neowizen");
    
    // Initial progress bar (empty)
    for (int i = 1; i <= 18; i++) {
        lcd.setCursor(i, 2);
        lcd.print(" ");
    }
    
    // Animate progress bar during initialization
    for (int step = 1; step <= 18; step++) {
        updateProgressBar(step, 18);
        delay(50);  // Smooth animation
    }
    
    delay(500);  // Show full progress bar briefly

    // Setup pins
    for (int i = 0; i < 4; i++)
        pinMode(PIN_DATAIN[i], OUTPUT);
    for (int i = 0; i < 2; i++) {
        pinMode(PIN_RD[i],  OUTPUT);
        pinMode(PIN_RR1[i], OUTPUT);
    }
    pinMode(PIN_CLOCK,    OUTPUT);
    pinMode(PIN_REGWRITE, OUTPUT);
    for (int i = 0; i < 4; i++)
        pinMode(PIN_RS1[i], INPUT);

    digitalWrite(PIN_CLOCK,    LOW);
    digitalWrite(PIN_REGWRITE, LOW);

    delay(500);

    Serial.println("\n");
    printDivider();
    Serial.println("              4-BIT REGISTER FILE TESTER");
    printDivider();

    // Run Unique Pattern Test
    Serial.println("\nRunning Unique Pattern Test...");
    runUniquePatternTest();
    
    // Run Exhaustive Test
    runExhaustiveTest();
    
    // Calculate final average for LCD
    float totalPct = 0.0f;
    for (int r = 0; r < 4; r++) {
        totalPct += regResults[r].pct;
    }
    float avgPct = totalPct / 4.0f;
    
    // Count perfect registers
    int perfect = 0;
    for (int r = 0; r < 4; r++) {
        if (regResults[r].pct == 100.0f)
            perfect++;
    }
    
    // LCD Final Report
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   TEST COMPLETED");
    lcd.setCursor(0, 1);
    lcd.print("   Result: ");
    lcd.print(avgPct, 1);
    lcd.print("%");
    lcd.setCursor(0, 2);
    lcd.print(" More bench details");
    lcd.setCursor(0, 3);
    lcd.print(" in Serial Terminal");
}

void loop() { 
    delay(200); 
}