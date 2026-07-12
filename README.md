# sunopen_mppt yaml
--- yaml
substitutions:
  device_name: sunopen-mppt
  friendly_name: SUNOPEN MPPT 太阳能控制器
  mppt_mac: "mPPT的蓝牙mac地址"
  wifi_ssid: "WiFi名称需要修改"
  wifi_password: "WiFi密码需要修改"

esphome:
  name: ${device_name}
  friendly_name: ${friendly_name}
  platformio_options:
    board_build.flash_mode: dio
  on_boot:
    priority: 600
    then:
      - wait_until: wifi.connected
      - delay: 3s
      - ble_client.connect: mppt_ble_client

esp32:
  board: esp32-s3-devkitc-1
  framework:
    type: esp-idf

wifi:
  ssid: ${wifi_ssid}
  password: ${wifi_password}
  fast_connect: true
  reboot_timeout: 15min

api:

ota:
  platform: esphome

logger:
  level: INFO
  baud_rate: 115200

external_components:
  - source: github://shenyuzi8/sunopen_mppt@main
    refresh: 0s

esp32_ble_tracker:
  scan_parameters:
    active: false

ble_client:
  - id: mppt_ble_client
    mac_address: ${mppt_mac}
    on_disconnect:
      # 只有开关打开时才自动重连
      - if:
          condition:
            switch.is_on: ble_enable_switch
          then:
            - logger.log: "MPPT 蓝牙断开，3秒后自动重连"
            - delay: 3s
            - ble_client.connect: mppt_ble_client
          else:
            - logger.log: "MPPT 蓝牙断开，开关已关闭，不重连"

sunopen_mppt:
  - id: mppt_device
    ble_client_id: mppt_ble_client
    throttle: 5s

sensor:
  - platform: sunopen_mppt
    sunopen_mppt_id: mppt_device
    battery_voltage:
      id: battery_voltage
      name: "01 电池电压"
    battery_current:
      id: battery_current
      name: "02 电池电流"
    battery_power:
      id: battery_power
      name: "03 电池功率"
    pv_voltage:
      id: pv_voltage
      name: "04 光伏电压"
    pv_current:
      id: pv_current
      name: "05 光伏电流"
    pv_power:
      id: pv_power
      name: "06 光伏功率"
    load_current:
      id: load_current
      name: "07 负载电流"
    load_power:
      id: load_power
      name: "08 负载功率"
    battery_soc:
      id: battery_soc
      name: "09 电池SOC"
    battery_temp:
      id: battery_temp
      name: "10 电池温度"
    controller_temp:
      id: controller_temp
      name: "11 控制器温度"
    today_energy:
      id: today_energy
      name: "12 今日发电量"
    charging_status:
      id: charging_status
      name: "13 充电状态"

switch:
  - platform: ble_client
    ble_client_id: mppt_ble_client
    name: "启用MPPT蓝牙连接"
    id: ble_enable_switch
    on_turn_off:
      - ble_client.disconnect: mppt_ble_client
      - logger.log: "手动断开MPPT蓝牙"
---
