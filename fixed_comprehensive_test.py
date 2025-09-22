#!/usr/bin/env python3
import socket
import time
import sys

def test_irc_server():
    print("🚀 Fixed Comprehensive IRC Server Test")
    print("=" * 50)
    
    results = []
    
    def log_test(test_name, success, message=""):
        status = "✅ PASS" if success else "❌ FAIL"
        print(f"{status} {test_name}: {message}")
        results.append((test_name, success, message))
    
    try:
        # Test 1: Basic Connection
        print("\n1. Testing Basic Connection...")
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect(('127.0.0.1', 6667))
        log_test("Connection", True, "Connected to server")
        
        # Test 2: Registration Process
        print("\n2. Testing Registration...")
        
        # PASS
        sock.send(b'PASS testpass\r\n')
        time.sleep(0.2)
        response = sock.recv(1024).decode()
        if "Welcome" in response:
            log_test("PASS", True, "Password accepted")
        else:
            log_test("PASS", False, f"Unexpected: {response.strip()}")
        
        # NICK (no response expected for success)
        sock.send(b'NICK testuser10\r\n')
        time.sleep(0.2)
        try:
            response = sock.recv(1024).decode()
            if "already" in response or "No nickname" in response:
                log_test("NICK", False, f"Error: {response.strip()}")
            else:
                log_test("NICK", True, "Nickname set (no response expected)")
        except socket.timeout:
            # Timeout is expected for successful NICK
            log_test("NICK", True, "Nickname set (no response expected)")
        
        # USER (no response expected for success)
        sock.send(b'USER testuser 0 * :Test User\r\n')
        time.sleep(0.2)
        try:
            response = sock.recv(1024).decode()
            if "Welcome to ft_irc" in response:
                log_test("USER", True, "Registration complete")
            else:
                log_test("USER", False, f"Unexpected: {response.strip()}")
        except socket.timeout:
            # Check if we're registered by trying a command
            sock.send(b'JOIN #test\r\n')
            time.sleep(0.2)
            try:
                response = sock.recv(1024).decode()
                if "JOIN" in response:
                    log_test("USER", True, "Registration complete (verified by JOIN)")
                else:
                    log_test("USER", False, "Registration failed")
            except socket.timeout:
                log_test("USER", False, "Registration failed")
        
        # Test 3: Channel Operations
        print("\n3. Testing Channel Operations...")
        
        # JOIN
        sock.send(b'JOIN #test\r\n')
        time.sleep(0.2)
        try:
            response = sock.recv(1024).decode()
            if "JOIN #test" in response:
                log_test("JOIN", True, "Successfully joined channel")
            else:
                log_test("JOIN", False, f"Unexpected: {response.strip()}")
        except socket.timeout:
            log_test("JOIN", False, "Timeout")
        
        # PRIVMSG to channel (no response expected)
        sock.send(b'PRIVMSG #test :Hello channel!\r\n')
        time.sleep(0.1)
        log_test("PRIVMSG Channel", True, "Message sent (no response expected)")
        
        # PART
        sock.send(b'PART #test\r\n')
        time.sleep(0.2)
        try:
            response = sock.recv(1024).decode()
            if "PART #test" in response:
                log_test("PART", True, "Successfully left channel")
            else:
                log_test("PART", False, f"Unexpected: {response.strip()}")
        except socket.timeout:
            log_test("PART", False, "Timeout")
        
        # Test 4: PING/PONG
        print("\n4. Testing PING/PONG...")
        sock.send(b'PING test123\r\n')
        time.sleep(0.2)
        try:
            response = sock.recv(1024).decode()
            if "PONG" in response:
                log_test("PING/PONG", True, "Pong received")
            else:
                log_test("PING/PONG", False, f"Unexpected: {response.strip()}")
        except socket.timeout:
            log_test("PING/PONG", False, "Timeout")
        
        # Test 5: Error Handling
        print("\n5. Testing Error Handling...")
        sock.send(b'PRIVMSG nonexistent :Message\r\n')
        time.sleep(0.2)
        try:
            response = sock.recv(1024).decode()
            if "No such nick" in response:
                log_test("Error Handling", True, "Correct error message")
            else:
                log_test("Error Handling", False, f"Unexpected: {response.strip()}")
        except socket.timeout:
            log_test("Error Handling", False, "Timeout")
        
        # QUIT
        sock.send(b'QUIT :Goodbye\r\n')
        time.sleep(0.2)
        log_test("QUIT", True, "Connection closed")
        
        sock.close()
        
    except Exception as e:
        log_test("Connection", False, f"Error: {str(e)}")
    
    # Summary
    print("\n" + "=" * 50)
    print("📊 TEST SUMMARY")
    print("=" * 50)
    
    passed = sum(1 for _, success, _ in results if success)
    failed = sum(1 for _, success, _ in results if not success)
    total = len(results)
    
    print(f"Total Tests: {total}")
    print(f"✅ Passed: {passed}")
    print(f"❌ Failed: {failed}")
    print(f"Success Rate: {(passed/total*100):.1f}%")
    
    if failed > 0:
        print("\n❌ Failed Tests:")
        for test_name, success, message in results:
            if not success:
                print(f"  - {test_name}: {message}")
    
    return passed == total

if __name__ == "__main__":
    success = test_irc_server()
    sys.exit(0 if success else 1)
