test_add_1:
  #_ REGISTER_IN r5 0x00100000
  #_ REGISTER_IN r25 0x0000FFFF

  add r11, r5, r25

  blr
  #_ REGISTER_OUT r5 0x00100000
  #_ REGISTER_OUT r25 0x0000FFFF
  #_ REGISTER_OUT r11 0x0010FFFF

test_add_2:
  #_ REGISTER_IN r0 0x00100000
  #_ REGISTER_IN r25 0x0000FFFF

  add r11, r0, r25

  blr
  #_ REGISTER_OUT r0 0x00100000
  #_ REGISTER_OUT r25 0x0000FFFF
  #_ REGISTER_OUT r11 0x0010FFFF
