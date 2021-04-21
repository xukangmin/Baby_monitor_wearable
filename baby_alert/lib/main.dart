import 'dart:math';

import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'dart:async';
//import 'package:flutter_blue/flutter_blue.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: MyHomePage(title: 'Flutter Demo Home Page'),
    );
  }
}

class LineChartDisplay extends StatelessWidget {
  LineChartDisplay({
    required this.dataInput
  });

  final List<FlSpot> dataInput;
  final List<Color> gradientColors = [
    const Color(0xff23b6e6),
    const Color(0xff02d39a),
  ];

  LineChartData mainData(List<FlSpot> data) {
    return LineChartData(
      gridData: FlGridData(
        show: true,
        drawVerticalLine: true,
        getDrawingHorizontalLine: (value) {
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
          showTitles: true,
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
          getTextStyles: (value) => const TextStyle(
            color: Color(0xff67727d),
            fontWeight: FontWeight.bold,
            fontSize: 15,
          ),
          getTitles: (value) {
            return value.toString();
          },
          reservedSize: 28,
          margin: 12,
        ),
      ),
      borderData: FlBorderData(
          show: true,
          border: Border.all(color: const Color(0xff37434d), width: 1)),
      minX: -10,
      maxX: 10,
      minY: -1,
      maxY: 1,
      lineBarsData: [
        LineChartBarData(
          spots: dataInput,
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
      ],
    );
  }

  @override
  Widget build(BuildContext context) {
    return Stack(
      children: <Widget>[
        AspectRatio(
          aspectRatio: 2,
          child: Container(
            decoration: const BoxDecoration(
                borderRadius: BorderRadius.all(
                  Radius.circular(5),
                ),
                color: Color(0xff232d37)), // back ground color
            child: Padding(
              padding: const EdgeInsets.only(
                  right: 18.0, left: 12.0, top: 24, bottom: 12),
              child: LineChart(
                 mainData(dataInput),
              ),
            ),
          ),
        ),
      ],
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
  // FlutterBlue flutterBlue = FlutterBlue.instance;
  // late BluetoothDevice bleDevice;
  final  List<FlSpot>  spots = [
  FlSpot(0, 3.12),
  FlSpot(2.6, 5),
  FlSpot(4.9, 1),
  FlSpot(6.8, 2),
  FlSpot(8, 3.44),
  FlSpot(9.5, 6),
  FlSpot(11, 13.44),
  ];

  void _incrementCounter() {
    setState(() {
      // This call to setState tells the Flutter framework that something has
      // changed in this State, which causes it to rerun the build method below
      // so that the display can reflect the updated values. If we changed
      // _counter without calling setState(), then the build method would not be
      // called again, and so nothing would appear to happen.
      _counter++;
    });
  }


  // void updateHeartRate(double val) {}
  //
  // Future<void> connectDevice(BluetoothDevice dev) async {
  //   await dev.connect();
  //   print("connected");
  //   List<BluetoothService> services = await dev.discoverServices();
  //   services.forEach((service) {
  //     print('service=${service.uuid.toString()}');
  //     if (service.uuid.toString() == '0000180d-0000-1000-8000-00805f9b34fb') {
  //       print("found heart rate service");
  //
  //       for (var char in service.characteristics) {
  //         print('char=${char.uuid.toString()}');
  //         if (char.uuid.toString() == '00002a37-0000-1000-8000-00805f9b34fb') {
  //           print("found characteristic");
  //           // char.setNotifyValue(true).whenComplete(() {
  //           //   char.value.listen((event) {
  //           //     print('event_length=${event.length}');
  //           //   });
  //           // });
  //         }
  //       }
  //     }
  //     // do something with service
  //   });
  // }

  @override
  initState() {
    super.initState();

    // flutterBlue.startScan(timeout: Duration(seconds: 5));
    //
    // flutterBlue.scanResults.listen((results) {
    //   // do something with scan results
    //   for (ScanResult r in results) {
    //     print('device=${r.device.id.id}');
    //     if (r.device.id.id == 'EC:E3:26:B6:EA:A0') {
    //       print("found device");
    //       bleDevice = r.device;
    //       //connectDevice(bleDevice);
    //
    //       break;
    //     }
    //   }
    // });
    //
    // flutterBlue.stopScan();
  }

  @override
  void dispose() {
    super.dispose();
    //bleDevice.disconnect();

    print("disposed");
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
            Padding(padding:  const EdgeInsets.only(right: 18.0, left: 12.0, top: 24, bottom: 12),
              child: LineChartDisplay(dataInput: spots),
            ),
            Text(
              'You have pushed the button this many times:',
            ),
            Text(
              '$_counter',
              style: Theme.of(context).textTheme.headline4,
            ),
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _incrementCounter,
        tooltip: 'Increment',
        child: Icon(Icons.add),
      ), // This trailing comma makes auto-formatting nicer for build methods.
    );
  }
}
