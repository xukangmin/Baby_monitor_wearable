import 'dart:math';

import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'dart:async';
import 'package:flutter_blue/flutter_blue.dart';

void main() {
  runApp(MyApp());
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
            fontSize: 20,
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
                      fontSize: 32,
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

class _MyHomePageState extends State<MyHomePage> {
  int _counter = 0;
  double _hr = 0;
  double _sp2 = 0;
  String _sensorStatus = "N/A";
  FlutterBlue flutterBlue = FlutterBlue.instance;
  late BluetoothDevice bleDevice;
  List<FlSpot> hrData = [];
  List<FlSpot> sp2Data = [];
  late Timer _timer;
  var rng = new Random();

  // void updateHeartRate(double val) {}
  //
  Future<void> connectDevice(BluetoothDevice dev) async {
    await dev.connect();
    print("connected");
    List<BluetoothService> services = await dev.discoverServices();
    services.forEach((service) {
      print('service=${service.uuid.toString()}');
      if (service.uuid.toString() == '0000180d-0000-1000-8000-00805f9b34fb') {
        print("found heart rate service");

        for (var char in service.characteristics) {
          print('char=${char.uuid.toString()}');
          if (char.uuid.toString() == '00002a37-0000-1000-8000-00805f9b34fb') {
            print("found characteristic");
            char.setNotifyValue(true).whenComplete(() {
              char.value.listen((event) {
                print('event_length=${event.length}');
                // update char here
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
    flutterBlue.startScan(timeout: Duration(seconds: 5));

    flutterBlue.scanResults.listen((results) {
      // do something with scan results
      for (ScanResult r in results) {
        print('device=${r.device.id.id}');
        if (r.device.id.id == 'EC:E3:26:B6:EA:A0') {
          print("found device");
          bleDevice = r.device;
          //connectDevice(bleDevice);

          break;
        }
      }
    });

    flutterBlue.stopScan();
  }

  @override
  void dispose() {
    super.dispose();
    bleDevice.disconnect();

    print("disposed1");
  }

  @override
  Widget build(BuildContext context) {
    // This method is rerun every time setState is called, for instance as done
    // by the _incrementCounter method above.
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
    return Scaffold(
      appBar: AppBar(
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        title: Text(widget.title),
      ),
      body: Center(
        // Center is a layout widget. It takes a single child and positions it
        // in the middle of the parent.
        child: Column(
          // Column is also a layout widget. It takes a list of children and
          // arranges them vertically. By default, it sizes itself to fit its
          // children horizontally, and tries to be as tall as its parent.
          //
          // Invoke "debug painting" (press "p" in the console, choose the
          // "Toggle Debug Paint" action from the Flutter Inspector in Android
          // Studio, or the "Toggle Debug Paint" command in Visual Studio Code)
          // to see the wireframe for each widget.
          //
          // Column has various properties to control how it sizes itself and
          // how it positions its children. Here we use mainAxisAlignment to
          // center the children vertically; the main axis here is the vertical
          // axis because Columns are vertical (the cross axis would be
          // horizontal).
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Padding(
              padding: const EdgeInsets.only(
                  right: 18.0, left: 12.0, top: 24, bottom: 12),
              child: LineChartDisplay(hrInput: hrData, sp2Input: sp2Data),
            ),
            Text(
              'Heart Rate:',
              style: TextStyle(fontSize: 20),
            ),
            Text(
              _hr.toStringAsFixed(1),
              style: Theme.of(context).textTheme.headline2,
            ),
            Text(
              'SPO2:',
              style: TextStyle(fontSize: 20),
            ),
            Text(
              _sp2.toStringAsFixed(1),
              style: Theme.of(context).textTheme.headline2,
            ),
            Text(
              'Sensor Status:',
              style: TextStyle(fontSize: 15),
            ),
            Text(
              _sensorStatus,
              style: Theme.of(context).textTheme.headline4,
            ),
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
