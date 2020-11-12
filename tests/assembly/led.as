start:
    li 1, 0
    # Store 0 to GPIO tri register, which configures GPIOs as output
    stw 1, 8196(0)

    # Fill GPR1 with 0xFFFFFFFF
    li 1, -1
    # Store GPR1 to GPIO data register
    stw 1, 8192(0)

    # Shift right to get the loop count 16777215 and store it to the count register
    srwi 2, 1, 8
    mtspr 9, 2

# Actual loop
loop:
    # Loop until the count is zero
    bdnz loop
    # Negate GPR1 and store it to GPIO data
    not 1, 1
    stw 1, 8192(0)
    # Set the count register to 16777215 again and jump back to the loop
    mtspr 9, 2
    b loop


