substitutions:
  device_name: sunopen-mppt
  friendly_name: SUNOPEN MPPT 太阳能控制器
  mppt_mac: "3A:13:00:75:47:69"
  wifi_ssid: "shenyuzi_2.4g"
  wifi_password: "ctanxin8"

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
  level: DEBUG
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
      - logger.log: "MPPT 蓝牙断开，3秒后自动重连"
      - delay: 3s
      - ble_client.connect: mppt_ble_client

sunopen_mppt:
  - id: mppt_device
    ble_client_id: mppt_ble_client
    throttle: 5s

sensor:
  - platform: sunopen_mppt
    sunopen_mppt_id: mppt_device
    battery_voltage:
      name: "电池电压"
    battery_current:
      name: "电池电流"
    battery_power:
      name: "电池功率"
    pv_voltage:
      name: "光伏电压"
    pv_current:
      name: "光伏电流"
    pv_power:
      name: "光伏功率"
    load_current:
      name: "负载电流"
    load_power:
      name: "负载功率"
    battery_soc:
      name: "电池SOC"
    battery_temp:
      name: "电池温度"
    controller_temp:
      name: "控制器温度"
    today_energy:
      name: "今日发电量"
    charging_status:
      name: "充电状态"

switch:
  - platform: ble_client
    ble_client_id: mppt_ble_client
    name: "启用MPPT蓝牙连接"
