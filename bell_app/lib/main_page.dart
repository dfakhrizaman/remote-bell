// ignore_for_file: prefer_const_constructors
import 'dart:convert';
import 'package:http/http.dart' as http;
import 'package:flutter/material.dart';

class MainPage extends StatefulWidget {
  const MainPage({Key? key}) : super(key: key);

  @override
  State<MainPage> createState() => _MainPageState();
}

class _MainPageState extends State<MainPage> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Bell App'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(
              'Press to ring the bell! 🔔',
              style: TextStyle(fontSize: 36),
            ),
            SizedBox(
              height: 24,
            ),
            ElevatedButton.icon(
              icon: Icon(
                Icons.doorbell_rounded,
                color: Colors.white,
                size: 32,
              ),
              label: Padding(
                padding: const EdgeInsets.all(8.0),
                child: Text(
                  'Ring Bell',
                  style: TextStyle(
                    color: Colors.white,
                    fontSize: 24,
                  ),
                ),
              ), // Your text here
              style: ButtonStyle(
                backgroundColor: MaterialStateProperty.all<Color>(
                    Theme.of(context).primaryColor),
              ),
              onPressed: () {
                Uri url = Uri.parse(
                    "https://mobile-doorbell-default-rtdb.asia-southeast1.firebasedatabase.app/bell_status.json");
                http
                    .put(
                      url,
                      body: json.encode(
                        {
                          "isBellPressed": "TRUE",
                        },
                      ),
                    )
                    // ignore: avoid_print
                    .then((response) => print(json.decode(response.body)));

                final snackBar = SnackBar(
                  content: const Text(
                    'Bell has been rung! 🔔🔔',
                    style: TextStyle(
                      fontSize: 20,
                    ),
                  ),
                  action: SnackBarAction(
                    label: 'Close',
                    onPressed: () {
                      // Some code to undo the change.
                    },
                  ),
                );

                ScaffoldMessenger.of(context).showSnackBar(snackBar);
              },
            ),
          ],
        ),
      ),
    );
  }
}
