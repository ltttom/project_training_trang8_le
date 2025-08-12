class MockHalSystemTsu {
    public:
        MOCK_METHOD0(hal_system_check_gpio_region_number, uint32_t());
        MOCK_METHOD0(hal_system_get_region_number, uint32_t());
        MOCK_METHOD1(hal_system_set_region_number, int32_t(uint32_t region_number));
        MOCK_METHOD0(hal_system_get_hw_type, uint32_t());
        MOCK_METHOD1(hal_system_set_hw_type, int32_t(uint32_t hw_type));
        MOCK_METHOD1(hal_system_get_supplier_id, int32_t(char* pbuff));
        MOCK_METHOD1(hal_system_get_part_number, int32_t(char* pbuff));
        MOCK_METHOD1(hal_system_get_drawing_number, int32_t(char* pbuff));
        MOCK_METHOD1(hal_system_get_spec_number, int32_t(char* pbuff));
        MOCK_METHOD1(hal_system_get_tsu_sw_version, int32_t(char* tsu_sw_version));
        MOCK_METHOD1(hal_system_get_serial_number, int32_t(char* pbuff));
        MOCK_METHOD2(hal_system_set_serial_number, int32_t(char* pbuff, uint32_t len));
};

MockHalSystemTsu *M_HalSystemTsu;

uint32_t hal_system_check_gpio_region_number() {
    return M_HalSystemTsu->hal_system_check_gpio_region_number();
}

uint32_t hal_system_get_region_number() {
    return M_HalSystemTsu->hal_system_get_region_number();
}

int32_t hal_system_set_region_number(uint32_t region_number) {
    return M_HalSystemTsu->hal_system_set_region_number(region_number);
}

uint32_t hal_system_get_hw_type() {
    return M_HalSystemTsu->hal_system_get_hw_type();
}

int32_t hal_system_set_hw_type(uint32_t hw_type) {
    return M_HalSystemTsu->hal_system_set_hw_type(hw_type);
}

int32_t hal_system_get_supplier_id(char* pbuff) {
    return M_HalSystemTsu->hal_system_get_supplier_id(pbuff);
}

int32_t hal_system_get_part_number(char* pbuff) {
    return M_HalSystemTsu->hal_system_get_part_number(pbuff);
}

int32_t hal_system_get_drawing_number(char* pbuff) {
    return M_HalSystemTsu->hal_system_get_drawing_number(pbuff);
}

int32_t hal_system_get_spec_number(char* pbuff) {
    return M_HalSystemTsu->hal_system_get_spec_number(pbuff);
}

int32_t hal_system_get_tsu_sw_version(char* tsu_sw_version) {
    return M_HalSystemTsu->hal_system_get_tsu_sw_version(tsu_sw_version);
}

int32_t hal_system_get_serial_number(char* pbuff) {
    return M_HalSystemTsu->hal_system_get_serial_number(pbuff);
}

int32_t hal_system_set_serial_number(char* pbuff, uint32_t len) {
    return M_HalSystemTsu->hal_system_set_serial_number(pbuff, len);
}


