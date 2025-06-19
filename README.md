Network Log Analyzer using Binary Trees:

-This C++ project parses and analyzes a bitacora.txt file containing log entries with IP addresses, timestamps, and error messages.

-The IPs are structured hierarchically into a custom binary tree: The first two octets (e.g. 192.168) represent networks, the last two octets (e.g. 1.34) represent hosts within that network, each host stores its logs in a separate binary search tree.

Features:

-Parses log entries and organizes them in a three-level tree structure: network → host → logs.

-Identifies the network(s) with the most hosts registered.

-Identifies the IP(s) with the most occurrences in the logs.

-Outputs both results in the required format.

Input: A file named bitacora.txt with 262k entries
