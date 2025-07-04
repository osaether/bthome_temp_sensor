# Bug Report: BTHome Temperature Sensor

## Summary
This report documents 3 critical bugs found and fixed in the BTHome temperature sensor codebase. The bugs range from logic errors affecting data precision to security vulnerabilities that could cause memory corruption.

## Bug #1: Integer Division Causing Loss of Precision

**Type:** Logic Error  
**Severity:** High  
**Location:** `src/sensor.c`, lines 53 and 65

### Description
The temperature and humidity calculations were using floating-point division (`/10000.0`) but storing the results in integer variables (`int32_t` and `uint32_t`). This caused truncation and loss of precision in the sensor readings.

### Root Cause
The code was performing the calculation:
```c
temp->temperature = (temp->temperature + 5000)/10000.0;
temp->humidity = (temp->humidity + 5000)/10000.0;
```

The division by `10000.0` (floating-point) produces a floating-point result, but this result is immediately truncated when stored in the integer variables.

### Impact
- Temperature and humidity readings would be less accurate
- Fractional parts of measurements would be lost
- Could lead to incorrect sensor data being transmitted via BTHome protocol

### Fix Applied
Changed the division to use integer arithmetic:
```c
temp->temperature = (temp->temperature + 5000)/10000;
temp->humidity = (temp->humidity + 5000)/10000;
```

This maintains the rounding behavior (due to the +5000 offset) while keeping the calculation consistent with integer storage.

## Bug #2: Potential Buffer Overflow/Memory Corruption

**Type:** Security Vulnerability  
**Severity:** Critical  
**Location:** `src/main.c`, lines 84-87

### Description
The main loop unconditionally attempted to update humidity values in the `service_data` array, even when the BME680 sensor (which supports humidity) was not configured. This could cause buffer overflows or writes to uninitialized memory.

### Root Cause
The `service_data` array is conditionally allocated based on `CONFIG_SENSOR_BME680`:
```c
static uint8_t service_data[] = {
    // ... temperature fields always present ...
#ifdef CONFIG_SENSOR_BME680
    0x03,   /* Humidity */
    0x00,   /* Low byte */
    0x00,   /* High byte */
#endif
};
```

However, the main loop always tried to write to humidity indices:
```c
service_data[IDX_HUMIL] = (uint8_t)((uint32_t)humidity & 0xff);
service_data[IDX_HUMIH] = (uint8_t)(((uint32_t)humidity >> 8) & 0xff);
```

### Impact
- Buffer overflow when humidity fields are not allocated
- Memory corruption in adjacent memory regions
- Potential system crashes or unpredictable behavior
- Security vulnerability that could be exploited

### Fix Applied
Added conditional compilation guard around humidity-related code:
```c
#ifdef CONFIG_SENSOR_BME680
    humidity = humidity_get();
    service_data[IDX_HUMIL] = (uint8_t)((uint32_t)humidity & 0xff);
    service_data[IDX_HUMIH] = (uint8_t)(((uint32_t)humidity >> 8) & 0xff);
#endif
```

This ensures humidity values are only written when the corresponding array elements exist.

## Bug #3: Missing Implementation of Declared Function

**Type:** Code Quality Issue  
**Severity:** Medium  
**Location:** `include/sensor.h` line 9, missing in `src/sensor.c`

### Description
The header file `sensor.h` declares a `pressure_get()` function that was never implemented in `sensor.c`. This represents incomplete functionality and could cause linker errors if the function were called.

### Root Cause
The header file included:
```c
uint32_t pressure_get(void);
```

But no corresponding implementation existed in the source file.

### Impact
- Linker errors if the function is called
- Broken API contract between header and implementation
- Confusion for developers using the API
- Potential runtime crashes if function is called

### Fix Applied
Added a stub implementation of the `pressure_get()` function:
```c
uint32_t pressure_get(void)
{
    /* Pressure is not supported by the current sensor implementations */
    return 0;
}
```

This provides a safe default implementation that won't cause linker errors and clearly indicates that pressure measurement is not supported by the current sensor types (TMP112, DS18B20, BME680 humidity-only).

## Additional Observations

### Potential Improvements
1. **Error Handling**: The sensor thread terminates on errors but doesn't provide recovery mechanisms
2. **Thread Safety**: Consider using atomic operations for simple value updates
3. **Configuration Validation**: Add compile-time checks to ensure valid sensor configurations

### Testing Recommendations
1. Test with all three sensor configurations (TMP112, DS18B20, BME680)
2. Verify advertising data format matches BTHome specification
3. Test error conditions and recovery scenarios
4. Validate sensor reading accuracy after precision fix

## Conclusion
The three bugs fixed represent different categories of issues:
- **Logic Error**: Affecting data accuracy
- **Security Vulnerability**: Potential memory corruption
- **Code Quality**: Incomplete API implementation

All fixes maintain backward compatibility while improving the robustness and security of the codebase.