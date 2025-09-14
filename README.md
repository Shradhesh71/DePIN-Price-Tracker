# ESP8266 DePIN Price Tracker

A decentralized physical infrastructure (DePIN) project that combines IoT temperature monitoring with Solana blockchain integration and real-time cryptocurrency price tracking.

## 🎯 Project Overview

This project demonstrates how Internet of Things (IoT) devices can interact with blockchain technology to create decentralized physical infrastructure. The system:

1. **Monitors Physical Environment** - Uses a DHT11 sensor to track temperature and humidity
2. **Tracks Cryptocurrency Prices** - Fetches real-time SOL/USD prices from Pyth Network oracles
3. **Blockchain Integration** - Automatically executes Solana transactions when temperature thresholds are exceeded
4. **Visual Feedback** - Uses LEDs to indicate system status and price/temperature conditions

## 🔧 Hardware Components

- **ESP8266 NodeMCU** - Main microcontroller with WiFi capability
- **DHT11 Sensor** - Temperature and humidity monitoring
- **LEDs** - Visual status indicators:
  - **Built-in LED (Blue)** - System heartbeat and WiFi status
  - **Red LED** - Temperature alert (>30°C)
  - **Blue LED** - SOL price indicator (>$100)

## 🌐 Why WiFi is Required

WiFi connectivity is essential for several core functions:

1. **Blockchain Communication** - Connect to Solana network for transactions
2. **Price Data** - Fetch real-time cryptocurrency prices from Pyth Network
3. **Network Time** - Synchronize timestamps for blockchain transactions
4. **Remote Monitoring** - Enable potential remote monitoring and control

## 🚀 How It Works

### System Flow

```
[DHT11 Sensor] → [ESP8266] → [WiFi] → [Solana Blockchain]
      ↓              ↓         ↓            ↓
[Temperature]  → [Processing] → [Internet] → [Smart Contract]
```

### Operational Logic

1. **Startup Sequence**
   - Initialize serial communication
   - Connect to WiFi network
   - Initialize sensors and LEDs
   - Fetch initial SOL balance and price

2. **Main Loop (Every 2 seconds)**
   - Read temperature and humidity from DHT11
   - Calculate heat index
   - Control LEDs based on conditions:
     - **Red LED ON**: Temperature > 30°C
     - **Blue LED ON**: SOL price > $100
   - Display readings on serial monitor

3. **Price Monitoring (Every 10 seconds)**
   - Fetch latest SOL/USD price from Pyth Network
   - Update LED status based on price

4. **Blockchain Interaction**
   - When temperature exceeds 30°C:
     - Execute `transferToVault()` function
     - Call `setTemp()` to record temperature on blockchain

## 📊 LED Status Indicators

| LED | Color | Status | Meaning |
|-----|-------|--------|---------|
| Built-in | Blue | Blinking | System running normally |
| External | Red | ON | Temperature above 30°C |
| External | Red | OFF | Temperature below 30°C |
| External | Blue | ON | SOL price above $100 |
| External | Blue | OFF | SOL price below $100 |

## 🔧 WiFi Setup and Troubleshooting

### Initial WiFi Configuration

1. **Update WiFi Credentials** in `src/main.cpp`:
```cpp
const char* ssid    = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

2. **Supported Networks**:
   - 2.4GHz WiFi networks only (ESP8266 limitation)
   - WPA/WPA2 security protocols
   - Open networks (not recommended)

### WiFi Connection Status

#### **Visual Indicators:**
- **Connecting**: Built-in LED may blink rapidly
- **Connected**: Serial monitor shows "WiFi connected" + IP address
- **Failed**: Stuck on "Connecting to [SSID]" with dots

#### **Serial Monitor Check:**
```
Connecting to YOUR_WIFI_SSID
..........
WiFi connected.
IP address: 192.168.1.XXX
```

### WiFi Troubleshooting

#### **Red LED Staying On - Possible Causes:**

1. **Temperature Actually High (>30°C)**
   - Check serial monitor for temperature readings
   - DHT11 sensor may be reading ambient temperature correctly
   - Solution: Cool the environment or adjust threshold in code

2. **DHT11 Sensor Issues**
   - Faulty sensor readings
   - Incorrect wiring
   - Check connections: VCC→3.3V, GND→GND, DATA→GPIO0

3. **WiFi Connection Problems**
   - Unable to fetch price data
   - Cannot execute blockchain transactions
   - System defaults to "alert" state

#### **WiFi Connection Failures:**

**Problem**: Cannot connect to WiFi
```
Connecting to YOUR_WIFI_SSID
................
(stuck forever)
```

**Solutions:**
1. **Check Network Compatibility**
   ```cpp
   // Ensure 2.4GHz network (not 5GHz)
   // Check SSID and password are correct
   ```

2. **Signal Strength Issues**
   - Move ESP8266 closer to router
   - Check for interference from other devices

3. **Router Settings**
   - Ensure SSID broadcast is enabled
   - Check MAC address filtering
   - Verify DHCP is enabled

4. **Code Debugging**
   ```cpp
   // Add to setup() for debugging:
   WiFi.printDiag(Serial);
   Serial.println(WiFi.status());
   ```

## 🔍 Serial Monitor Debugging

### Normal Operation Output:
```
ESP8266 NodeMCU Price Tracker Example
Built-in LED will start blinking...
Connecting to YOUR_WIFI_SSID
..........
WiFi connected.
IP address: 192.168.1.100

=== 🔹 getSolBalance() ===
Wallet address: AHYic562KhgtAEkb1rSesqS87dFYRcfXb4WwWus3Zc9C
SOL Balance (lamports): 1000000
SOL Balance (SOL): 0.001000000

=== Fetching initial Solana price ===
Fetching price from: https://hermes.pyth.network/api/latest_price_feeds?ids[]=0xef0d8b6fda2ceba41da15d4095d1da392a0d2f8ed0c6c7bc0f4cfac8c280b56d
Solana Price Data:
Current Price: $150.25
Confidence: ±$0.05

Setup complete.

LEDs ON
LEDs OFF
Humidity: 45.00%  Temperature: 25.30°C 77.54°F  Heat index: 24.85°C 76.73°F
Heat index is below 30°C :)
```

### Error Indicators:
```
Failed to read from DHT sensor!          // Sensor connection issue
Failed to fetch Solana price            // Network/API issue
❌ Failed to fetch SOL balance           // Blockchain connection issue
❌ Failed to get blockhash!              // Network issue
```

## ⚙️ Configuration

### Blockchain Settings (src/main.cpp):
```cpp
const String solanaRpcUrl = "https://api.devnet.solana.com";
const String PRIVATE_KEY_BASE58 = "YOUR_PRIVATE_KEY";
const String PUBLIC_KEY_BASE58 = "YOUR_PUBLIC_KEY";
```

### Temperature Threshold:
```cpp
if (hic > 30) {  // Change 30 to desired temperature in Celsius
    // Temperature alert logic
}
```

### Price Threshold:
```cpp
if (solPrice.price > 100.0) {  // Change 100.0 to desired USD price
    // Price alert logic
}
```

## 🔧 Hardware Setup

Refer to `ESP8266_Hardware_Setup.md` for detailed wiring instructions.

### Quick Connection Summary:
```
DHT11 Sensor:
├── VCC → 3.3V
├── GND → GND
└── DATA → GPIO0 (D3)

Red LED:
├── Anode (+) → 220Ω resistor → GPIO5 (D1)
└── Cathode (-) → GND

Blue LED:
├── Anode (+) → 220Ω resistor → GPIO4 (D2)
└── Cathode (-) → GND
```

## 🛠️ Development Setup

1. **Install PlatformIO** (VS Code extension recommended)
2. **Clone Repository**
3. **Update Configuration**:
   - WiFi credentials
   - Solana wallet addresses
   - API endpoints
4. **Build and Upload**:
   ```bash
   platformio run --environment esp8266 --target upload
   ```
5. **Monitor Serial Output**:
   ```bash
   platformio device monitor
   ```

## 📈 Use Cases

### 1. **Smart Agriculture**
- Monitor greenhouse temperature
- Automatic irrigation triggers
- Weather-based crop insurance

### 2. **Supply Chain Monitoring**
- Temperature-sensitive shipments
- Cold chain verification
- Quality assurance automation

### 3. **Smart Buildings**
- HVAC automation
- Energy efficiency tracking
- Occupancy-based climate control

### 4. **Environmental Monitoring**
- Air quality tracking
- Climate research data
- Pollution monitoring

## 🔐 Security Considerations

1. **Private Key Management**
   - Never commit private keys to version control
   - Use environment variables in production
   - Consider hardware security modules

2. **Network Security**
   - Use WPA2/WPA3 encrypted WiFi
   - Consider VPN for sensitive applications
   - Regular firmware updates

3. **Data Validation**
   - Sensor reading verification
   - Price data validation
   - Transaction confirmation

## 🐛 Common Issues & Solutions

### Issue: Red LED Always On
**Cause**: Temperature reading above 30°C or sensor malfunction
**Solution**: Check actual temperature, verify sensor wiring, adjust threshold

### Issue: No WiFi Connection
**Cause**: Wrong credentials, network issues, or 5GHz network
**Solution**: Verify 2.4GHz network, check credentials, improve signal strength

### Issue: No Price Data
**Cause**: Internet connectivity or API rate limiting
**Solution**: Check WiFi, verify API endpoints, implement retry logic

### Issue: Blockchain Transactions Fail
**Cause**: Network congestion, insufficient balance, or wrong addresses
**Solution**: Check SOL balance, verify network status, update RPC endpoint

## 📚 Additional Resources

- [ESP8266 Hardware Setup Guide](ESP8266_Hardware_Setup.md)
- [ESP8266 Testing Guide](ESP8266_Setup_Testing_Guide.md)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [Solana Documentation](https://docs.solana.com/)
- [Pyth Network](https://pyth.network/)

## 🤝 Contributing

1. Fork the repository
2. Create feature branch
3. Make changes
4. Test thoroughly
5. Submit pull request

## 📄 License

This project is open source and available under the MIT License.

---

**Need Help?** Check the serial monitor output and compare with the examples above to diagnose issues.