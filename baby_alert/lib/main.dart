import 'dart:convert';
import 'dart:io';
import 'dart:math';

import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/services.dart';
import 'dart:async';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'dart:typed_data';

import 'package:mqtt_client/mqtt_server_client.dart';

void main() {
  runApp(MyApp());
}

class HealthData {
  final double value;

  HealthData(this.value);

  Map<String, dynamic> toJson() => {'value': value};
}

class HealthDataString {
  final String value;

  HealthDataString(this.value);

  Map<String, dynamic> toJson() => {'value': value};
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData.dark(),
      home: MyHomePage(title: 'Baby Monitor'),
    );
  }
}

class LineChartDisplay extends StatelessWidget {
  LineChartDisplay({required this.hrInput, required this.sp2Input});

  final List<FlSpot> hrInput;
  final List<FlSpot> sp2Input;
  final List<Color> gradientColors = [
    const Color(0xff23b6e6),
    const Color(0xff02d39a),
  ];

  final List<Color> gradientColors1 = [
    const Color(0xffe623cc),
    const Color(0xffffe67a),
  ];

  double getMinValX(List<FlSpot> d) {
    if (d.isEmpty) return 0;
    double val = d[0].x;

    for (var v in d) {
      if (v.x < val) {
        val = v.x;
      }
    }

    return val;
  }

  double getMinValY(List<FlSpot> d) {
    if (d.isEmpty) return 0;
    double val = d[0].y;

    for (var v in d) {
      if (v.y < val) {
        val = v.y;
      }
    }

    return val;
  }

  double getMaxValY(List<FlSpot> d) {
    print("called");
    if (d.isEmpty) return 0;
    double val = d[0].y;

    for (var v in d) {
      if (v.y > val) {
        val = v.y;
      }
    }
    print('max_y=$val');
    return val;
  }

  double getMaxValX(List<FlSpot> d) {
    if (d.isEmpty) return 0;
    double val = d[0].x;

    for (var v in d) {
      if (v.x > val) {
        val = v.x;
      }
    }

    return val;
  }

  LineChartData mainData(List<FlSpot> hrData, List<FlSpot> sp2Data) {
    return LineChartData(
      gridData: FlGridData(
        show: false,
        drawVerticalLine: true,
        drawHorizontalLine: true,
        checkToShowHorizontalLine: (value) {
          //print(value);
          return true;
        },
        checkToShowVerticalLine: (value) {
          //print(value);
          return true;
        },
        getDrawingHorizontalLine: (value) {
          //print(value);
          return FlLine(
            color: const Color(0xff37434d),
            strokeWidth: 1,
          );
        },
        getDrawingVerticalLine: (value) {
          return FlLine(
            color: const Color(0xff37434d),
            strokeWidth: 1,
          );
        },
      ),
      titlesData: FlTitlesData(
        show: true,
        bottomTitles: SideTitles(
          showTitles: false,
          interval: 2,
          reservedSize: 22,
          getTextStyles: (value) => const TextStyle(
              color: Color(0xff68737d),
              fontWeight: FontWeight.bold,
              fontSize: 16),
          getTitles: (value) {
            return value.toString();
          },
          margin: 8,
        ),
        leftTitles: SideTitles(
          showTitles: true,
          interval: 10,
          getTextStyles: (value) => const TextStyle(
            color: Color(0xff67727d),
            fontWeight: FontWeight.bold,
            fontSize: 15,
          ),
          getTitles: (value) {
            //print(value)
            return value.toString();
          },
          reservedSize: 22,
          margin: 20,
        ),
      ),
      borderData: FlBorderData(
          show: true,
          border: Border.all(color: const Color(0xff37434d), width: 1)),
      minX: getMinValX(hrData),
      maxX: getMaxValX(hrData),
      minY: 40,
      maxY: 150,
      lineBarsData: [
        LineChartBarData(
          spots: hrData,
          isCurved: true,
          colors: gradientColors,
          barWidth: 2,
          isStrokeCapRound: true,
          dotData: FlDotData(
            show: false,
          ),
          belowBarData: BarAreaData(
            show: false,
            colors:
                gradientColors.map((color) => color.withOpacity(0.3)).toList(),
          ),
        ),
        LineChartBarData(
          spots: sp2Data,
          isCurved: true,
          colors: gradientColors1,
          barWidth: 2,
          isStrokeCapRound: true,
          dotData: FlDotData(
            show: false,
          ),
          belowBarData: BarAreaData(
            show: false,
            colors:
                gradientColors1.map((color) => color.withOpacity(0.3)).toList(),
          ),
        ),
      ],
    );
  }

  @override
  Widget build(BuildContext context) {
    return AspectRatio(
      aspectRatio: 1.23,
      child: Container(
        decoration: const BoxDecoration(
          borderRadius: BorderRadius.all(Radius.circular(18)),
          gradient: LinearGradient(
            colors: [
              Color(0xff2c274c),
              Color(0xff46426c),
            ],
            begin: Alignment.bottomCenter,
            end: Alignment.topCenter,
          ),
        ),
        child: Stack(
          children: <Widget>[
            Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: <Widget>[
                const SizedBox(
                  height: 4,
                ),
                const Text(
                  'Heart Rate - SPO2 Chart',
                  style: TextStyle(
                      color: Colors.white,
                      fontSize: 28,
                      fontWeight: FontWeight.bold,
                      letterSpacing: 2),
                  textAlign: TextAlign.center,
                ),
                const SizedBox(
                  height: 37,
                ),
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.only(right: 16.0, left: 20.0),
                    child: LineChart(
                      mainData(hrInput, sp2Input),
                      swapAnimationDuration: const Duration(milliseconds: 250),
                    ),
                  ),
                ),
                const SizedBox(
                  height: 10,
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}

class MyHomePage extends StatefulWidget {
  MyHomePage({Key? key, required this.title}) : super(key: key);

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class AwsMQTT {
  late MqttServerClient client;

  Future<void> init() async {
    print("init aws mqtt");

    ByteData caData = await rootBundle.load('certs/AmazonRootCA.pem');
    ByteData certData =
        await rootBundle.load('certs/awsiot-certificate.pem.crt');
    ByteData privateKeyData =
        await rootBundle.load('certs/awsiot-private.pem.key');

    var _iotEndpoint = 'a3e4d2huogr3x6-ats.iot.us-east-1.amazonaws.com';
    var _clientId = '123123';

    client = MqttServerClient.withPort(_iotEndpoint, _clientId, 8883,
        maxConnectionAttempts: 5);
    client.logging(on: true);
    client.keepAlivePeriod = 20;
    client.secure = true;

    final securityContext = SecurityContext.defaultContext;

    securityContext.useCertificateChainBytes(certData.buffer.asUint8List());
    securityContext.setClientAuthoritiesBytes(caData.buffer.asUint8List());
    securityContext.usePrivateKeyBytes(privateKeyData.buffer.asUint8List());

    client.securityContext = securityContext;
    client.setProtocolV311();

    final MqttConnectMessage connMess = MqttConnectMessage()
        .withClientIdentifier(_clientId)
        .startClean()
        .keepAliveFor(30);

    client.connectionMessage = connMess;

    try {
      print('MQTTClientWrapper::Mosquitto client connecting....');
      await client.connect();
    } on Exception catch (e) {
      print('MQTTClientWrapper::client exception - $e');
      client.disconnect();
    }
  }

  awsMQTT() {}

  Future<void> publish(String topic, String msg) async {
    if (client.connectionStatus!.state == MqttConnectionState.connected) {
      final builder1 = MqttClientPayloadBuilder();

      builder1.addString(msg);

      client.publishMessage(topic, MqttQos.atLeastOnce, builder1.payload!);
    } else {
      print(
          'MQTTClientWrapper::ERROR Mosquitto client connection failed - disconnecting, status is ${client.connectionStatus}');
      client.disconnect();
    }
  }

  void uninit() {
    client.disconnect();
  }
}

class _MyHomePageState extends State<MyHomePage> {
  int _hr = 0;
  int _sp2 = 0;
  int _sensorStatus = 0;
  int _confidence = 0;
  int _counter = 0;
  double _temp = 0;
  FlutterBlue flutterBlue = FlutterBlue.instance;
  late BluetoothDevice bleDevice;
  bool isFound = false;
  List<FlSpot> hrData = [];
  List<FlSpot> sp2Data = [];
  late Timer _timer;
  var rng = new Random();
  String _strBleStatus = 'N/A';
  String _strSensorStatus = 'N/A';
  int isInit = 0;
  AwsMQTT awsClient = new AwsMQTT();

  Future<int> publishStatus(String status) async {
    awsClient.publish(
        'babyAlert/string/sensorStatus', jsonEncode(HealthDataString(status)));

    return 0;
  }

  Future<int> publishTemp(double temp) async {
    awsClient.publish(
        'babyAlert/double/bodyTemp', jsonEncode(HealthData(temp.toDouble())));

    return 0;
  }

  Future<int> publishData(
      int hr, int spo2, int sensorStatus, int confidence) async {
    awsClient.publish(
        'babyAlert/int/hr', jsonEncode(HealthData(hr.toDouble())));

    awsClient.publish(
        'babyAlert/int/spo2', jsonEncode(HealthData(spo2.toDouble())));

    awsClient.publish('babyAlert/int/status',
        jsonEncode(HealthData(sensorStatus.toDouble())));

    awsClient.publish(
        'babyAlert/int/conf', jsonEncode(HealthData(confidence.toDouble())));

    return 0;
  }

  Future<void> connectDevice(BluetoothDevice dev) async {
    await dev.connect();
    print("connected");
    List<BluetoothService> services = await dev.discoverServices();
    services.forEach((service) {
      print('service=${service.uuid.toString()}');
      if (service.uuid.toString() == '12345678-1234-5678-1234-56789abcdef0') {
        print("found heart rate service");

        for (var char in service.characteristics) {
          print('char=${char.uuid.toString()}');
          if (char.uuid.toString() == '12345678-1234-5678-1234-56789abcdef1') {
            print("found characteristic");
            char.setNotifyValue(true).whenComplete(() {
              char.value.listen((event) {
                if (event.length == 8) {
                  print('heart rate = ${event[0]}');
                  print('spo2 = ${event[1]}');
                  print('status = ${event[2]}');
                  print('confidence = ${event[3]}');

                  ByteData fdata = new ByteData(4);

                  fdata.setUint8(0, event[7]);

                  fdata.setUint8(1, event[6]);
                  fdata.setUint8(2, event[5]);
                  fdata.setUint8(3, event[4]);

                  _temp = fdata.getFloat32(0);

                  print('temp = $_temp');

                  publishTemp(_temp);

                  setState(() {
                    _hr = event[0];
                    _sp2 = event[1];
                    _sensorStatus = event[2];
                    _confidence = event[3];

                    _strBleStatus = 'Connected';

                    switch (_sensorStatus) {
                      case 0:
                        _strSensorStatus = 'No Contact';
                        break;
                      case 1:
                        _strSensorStatus = 'Contact';
                        break;
                      case 2:
                      case 3:
                        _strSensorStatus = 'Stable';
                        break;
                    }

                    publishStatus(_strSensorStatus);

                    if (_sensorStatus == 3 &&
                        _hr > 40 &&
                        _hr < 180 &&
                        _sp2 > 80 &&
                        _sp2 <= 100) {
                      // valid data
                      _counter++;
                      var hrspot =
                          new FlSpot(_counter.toDouble(), _hr.toDouble());
                      hrData.add(hrspot);
                      var sp2spot =
                          new FlSpot(_counter.toDouble(), _sp2.toDouble());

                      sp2Data.add(sp2spot);

                      publishData(_hr, _sp2, _sensorStatus, _confidence);
                    }
                  });
                }
              });
            });
          }
        }
      }
      // do something with service
    });
  }

  @override
  initState() {
    super.initState();

    // _timer = new Timer.periodic(Duration(seconds: 1), (timer) {
    //   setState(() {
    //     _counter++;
    //     _hr = rng.nextDouble() * 50 + 50;
    //     var hr = new FlSpot(_counter.toDouble(), _hr);
    //     if (hrData.length > 20) {
    //       hrData.removeAt(0);
    //     }

    //     hrData.add(hr);

    //     _sp2 = rng.nextDouble() * 10 + 90;
    //     var sp2 = new FlSpot(_counter.toDouble(), _sp2);
    //     if (sp2Data.length > 20) {
    //       sp2Data.removeAt(0);
    //     }

    //     sp2Data.add(sp2);
    //   });
    // });
    //
    isFound = false;

    setState(() {
      _strBleStatus = 'Scanning';
    });

    flutterBlue.startScan(timeout: Duration(seconds: 5)).whenComplete(() => {
          if (isFound)
            awsClient.init().whenComplete(() => connectDevice(bleDevice))
        });

    flutterBlue.scanResults.listen((results) {
      // do something with scan results
      for (ScanResult r in results) {
        print('device=${r.device.id.id}');
        if (r.device.id.id == 'EC:E3:26:B6:EA:A0') {
          print("found device");
          setState(() {
            _strBleStatus = 'Found Device';
          });
          bleDevice = r.device;
          isFound = true;

          break;
        }
      }
    });
  }

  @override
  void dispose() {
    super.dispose();
    bleDevice.disconnect();
    awsClient.uninit();
    print("disposed1");
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Padding(
              padding: const EdgeInsets.only(
                  right: 18.0, left: 12.0, top: 24, bottom: 12),
              child: LineChartDisplay(hrInput: hrData, sp2Input: sp2Data),
            ),
            Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
              Column(
                children: [
                  Text(
                    'Heart Rate',
                    style: TextStyle(fontSize: 18),
                  ),
                  Text(
                    _hr.toStringAsFixed(0),
                    style: Theme.of(context).textTheme.headline2,
                  ),
                ],
              ),
              Column(
                children: [
                  Text(
                    'Oxygen',
                    style: TextStyle(fontSize: 18),
                  ),
                  Text(
                    _sp2.toStringAsFixed(0),
                    style: Theme.of(context).textTheme.headline2,
                  ),
                ],
              ),
              Column(
                children: [
                  Text(
                    'Body Temp',
                    style: TextStyle(fontSize: 18),
                  ),
                  Text(
                    _temp.toStringAsFixed(1),
                    style: Theme.of(context).textTheme.headline2,
                  ),
                ],
              )
            ]),
            Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
              Column(
                children: [
                  Text(
                    'Sensor Status:',
                    style: TextStyle(fontSize: 15),
                  ),
                  Text(
                    _strSensorStatus,
                    style: Theme.of(context).textTheme.headline4,
                  )
                ],
              ),
              Column(
                children: [
                  Text(
                    'BLE Status:',
                    style: TextStyle(fontSize: 15),
                  ),
                  Text(
                    _strBleStatus,
                    style: Theme.of(context).textTheme.headline4,
                  )
                ],
              )
            ])
          ],
        ),
      ),
    );
  }

  @override
  void debugFillProperties(DiagnosticPropertiesBuilder properties) {
    super.debugFillProperties(properties);
    properties.add(DiagnosticsProperty<Timer>('_timer', _timer));
  }
}
