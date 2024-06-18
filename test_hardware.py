import time
import unittest

from threading import Thread
import os

import serial

from utils import util
import config

class Linkage(unittest.TestCase):
    encoder_pos = [0, 0, 0, 0, 0, 0]
    continue_serial_connection_flag = True

    def test_mechanical(self):
        res = util.upload_firmware('./firmware/hardware/linkage encoder')
        self.assertEqual(res, 0, msg='failed to upload firmware')
        time.sleep(1)
        self.continue_serial_connection_flag = True
        serial_connection_thread = Thread(target=self.handle_serial_connection)
        serial_connection_thread.start()

        # Upper handle

        print("#### Upper Handle ####")
        print("move the upperhandle to the default position [ENTER]")
        input()
        start = self.encoder_pos
        print("move the upperhandle to the rightmost position [ENTER]")
        input()
        mid = self.encoder_pos
        print("move the upperhandle to the start position [ENTER]")
        input()
        end = self.encoder_pos
        print(start)
        print(mid)
        print(end)
        self.assertLess(abs(start[0] - end[0]), 500, "start and end position are not aligning")
        self.assertLess(abs(start[1] - end[1]), 500, "start and end position are not aligning")
        self.assertTrue(3500 < abs(start[0] - mid[0]) < 4500, "the left encoder of the upper handle didn't move far enough")
        self.assertTrue(3500 < abs(start[1] - mid[1]) < 4500, "the right encoder of the upper handle didn't move far enough")

        #Lower handle

        print("#### Lower Handle ####")
        print("move the lower handle to the default position [ENTER]")
        input()
        start = self.encoder_pos
        print("move the lower handle to the leftmost position [ENTER]")
        input()
        mid = self.encoder_pos
        print("move the lower handle to the start position [ENTER]")
        input()
        end = self.encoder_pos
        print(start)
        print(mid)
        print(end)
        self.assertLess(abs(start[2] - end[2]), 500, "start and end position are not aligning")
        self.assertLess(abs(start[3] - end[3]), 500, "start and end position are not aligning")
        self.assertTrue(3500 < abs(start[2] - mid[2]) < 4500,
                        "the left encoder of the upper handle didn't move far enough")
        self.assertTrue(3500 < abs(start[3] - mid[3]) < 4500,
                        "the right encoder of the upper handle didn't move far enough")

        # close serial connection
        self.continue_serial_connection_flag = False

    def test_motor(self):
        res = util.upload_firmware('./firmware/hardware/linkage motor')
        self.assertEqual(res, 0, msg='failed to upload firmware')
        time.sleep(1)
        print("Are both linkages oscillating forwards and backwards without drifting to one side? [y / N]")
        x = input()
        if x[0] == "y" or x[0] == "Y":
            return
        print("Are both linkages oscillating forwards and backwards but one or both are drifting to one side? [y / N]")
        if x[0] == "y" or x[0] == "Y":
            print("Are -all- of the motors working (Doing something)? [y / N]")
            if x[0] == "y" or x[0] == "Y":
                print("""If all of the motors seem to work then that's not an error but check if the linkage is rubbing 
                against the case and keep in mind that if a problem occurs it could have something to do with one of the
                motors being weaker than the others""")
                return
            print("One motor might be broken. Ask your tutor.")
            return
        print("Is the battery connected and the switch turned on? [y / N]")
        if x[0] == "y" or x[0] == "Y":
            print("Please ask your tutor.")

    def test_motor_encoder_combination(self):
        print("Move the handles to the default position.")
        res = util.upload_firmware('./firmware/hardware/linkage encoder motor combination')
        self.assertEqual(res, 0, msg='failed to upload firmware')
        time.sleep(1)
        with serial.Serial(config.COM_PORT, 9600, timeout=1, parity=serial.PARITY_EVEN) as ser:

            x = lambda a: [int(y) for y in a.split(",")[:-1]]

            points = []
            while True:
                if ser.inWaiting() < 10:
                    continue
                points.append(x(ser.readline().decode("utf-8")))
                time.sleep(0.01)
                if len(points) == 6:
                    break
            print(points)
            for i in range(1, len(points)):
                for j in range(len(points[i])):
                    if points[i - 1][j] - points[i][j] > 10000:
                        points[i][j] += 16834
                    if points[i - 1][j] - points[i][j] < -10000:
                        points[i][j] -= 16834

            print(points)

            self.assertLess(abs(points[0][0] - points[2][0]), 1000, "start and end position are not aligning")
            self.assertLess(abs(points[0][1] - points[2][1]), 1000, "start and end position are not aligning")
            self.assertTrue(3000 < abs(points[0][0] - points[1][0]) < 5000,
                            "the left encoder of the upper handle didn't move far enough")
            self.assertTrue(3000 < abs(points[0][1] - points[1][1]) < 5000,
                            "the right encoder of the upper handle didn't move far enough")

            self.assertLess(abs(points[3][2] - points[5][2]), 1000, "start and end position are not aligning")
            self.assertLess(abs(points[3][3] - points[5][3]), 1000, "start and end position are not aligning")
            self.assertTrue(3000 < abs(points[3][2] - points[4][2]) < 5000,
                            "the left encoder of the upper handle didn't move far enough")
            self.assertTrue(3000 < abs(points[3][3] - points[4][3]) < 5000,
                            "the right encoder of the upper handle didn't move far enough")

    def test_sync(self):
        res = util.upload_firmware('./firmware/hardware/linkage sync')
        self.assertEqual(res, 0, msg='failed to upload firmware')
        time.sleep(1)
        # with serial.Serial(config.COM_PORT, 9600, timeout=1) as ser:
        #     while True:
        #         message = b'dpmotor100234;'
        #         ser.write(message)
        # res = ser.read(1)
        # print(res)

    def handle_serial_connection(self):
        print("Connecting...")
        with serial.Serial(config.COM_PORT, 9600, timeout=1, parity=serial.PARITY_EVEN) as ser:
            time.sleep(1)
            self.assertNotEqual(ser.inWaiting(), 0,
                                msg="could not establish serial connection... try restarting the panto")
            uint_overflow_correction = [0, 0, 0, 0]
            while self.continue_serial_connection_flag:
                if ser.inWaiting() > 0:
                    r = str(ser.readline()).split("dptest")
                    if len(r) != 2:
                        print(r, " has wrong serial format - skipping")
                        continue
                    new_encoder_pos = [int(y) for y in [x.split(",")[:-1] for x in r][1]]
                    #print(new_encoder_pos)
                    # correcting the uint overflow -> 16383 (14bit max) jump to 0
                    for i in range(4):
                        if self.encoder_pos[i] - uint_overflow_correction[i] - new_encoder_pos[i] > 10000:
                            uint_overflow_correction[i] += 16383
                        if self.encoder_pos[i] - uint_overflow_correction[i] - new_encoder_pos[i] < -10000:
                            uint_overflow_correction[i] -= 16383
                        new_encoder_pos[i] += uint_overflow_correction[i]
                    self.encoder_pos = new_encoder_pos
                    # print(self.encoder_pos)
                else:
                    time.sleep(0.01)



class EndEffector(unittest.TestCase):
    def test_encoder(self):
        res = util.upload_firmware('./firmware/04 encoder read')
        self.assertEqual(res, 0, msg='failed to upload firmware')
        time.sleep(1)
        # move serial connection handling to other thread
        serial_connection_thread = Thread(target=self.handle_serial_connection)
        serial_connection_thread.start()

        time.sleep(3)
        # self.assertEqual(len(self.encoder_pos), 4, msg="getting no data from connection thread")
        start_position = self.encoder_pos

        display_interval = 7500
        display_step_size = 200

        try:
            while serial_connection_thread.is_alive():
                rel_pos = [self.encoder_pos[i] - start_position[i] for i in range(len(self.encoder_pos))]
                print("Move the handles to test the encoders")
                print(rel_pos)
                print(" " * int(display_interval / display_step_size), "|")
                # for j in range(4): # four main encoders
                #     b = True
                #     for i in range(-display_interval, display_interval, display_step_size):
                #         if rel_pos[j] < i and b:
                #             b = False
                #             print("#", end="")
                #         else:
                #             print(" ", end="")
                #     if b:
                #         print("#", end="")
                #     print()
                # print()
                # for j in range(4, 6): # two endeffector encoders
                #     print("#" * (rel_pos[j] // 4))
                # print()
                print("Press CTRL + C to continue")
                time.sleep(0.05)
                os.system('cls' if os.name == 'nt' else 'clear')
        except KeyboardInterrupt:
            self.continue_serial_connection_flag = False

    def test_motor(self):
        res = util.upload_firmware('./firmware/linkage motor')
        self.assertEqual(res, 0, msg='failed to upload firmware')
        time.sleep(1)
        print("Press any key to continue")
        input()

    def test_sync(self):
        res = util.upload_firmware('./firmware/hardware/linkage sync')
        self.assertEqual(res, 0, msg='failed to upload firmware')
        time.sleep(1)
        # with serial.Serial(config.COM_PORT, 9600, timeout=1) as ser:
        #     while True:
        #         message = b'dpmotor100234;'
        #         ser.write(message)
                #res = ser.read(1)
                #print(res)

    def handle_serial_connection(self):
        print("Connecting...")
        with serial.Serial(config.COM_PORT, 115200, timeout=1, parity=serial.PARITY_EVEN) as ser:
            time.sleep(1)
            self.assertNotEqual(ser.inWaiting(), 0, msg="could not establish serial connection... try restarting the panto")
            while self.continue_serial_connection_flag:
                if ser.inWaiting() > 0:
                    r = str(ser.readline()).split("dptest")
                    if len(r) != 2:
                        print(r, " has wrong serial format - skipping")
                        continue
                    new_encoder_pos = [int(y) for y in [x.split(",")[:-1] for x in r][1]]
                    for i in range(4, 6):
                        # 136 steps for the endeffector encoders
                        new_encoder_pos[i] = abs(new_encoder_pos[i] % (136 * 2))
                    self.encoder_pos = new_encoder_pos
                    #print(self.encoder_pos)
                else:
                    time.sleep(0.01)

class HardwareTest(unittest.TestCase):
    encoder_pos = [0, 0, 0, 0]
    continue_serial_connection_flag = True

    def test_motor(self):
        res = util.upload_firmware('./firmware/03 motor')
        self.assertEqual(res, 0, msg='failed to upload firmware')
        time.sleep(1)
        print("Press any key to continue")
        input()

    def handle_serial_connection(self):
        print("Connecting...")
        with serial.Serial(config.COM_PORT, 9600, timeout=1, parity=serial.PARITY_EVEN) as ser:
            time.sleep(1)
            self.assertNotEqual(ser.inWaiting(), 0, msg="could not establish serial connection... try restarting the panto")
            uint_overflow_correction = [0,0,0,0]
            while self.continue_serial_connection_flag:
                if ser.inWaiting() > 0:
                    r = str(ser.readline()).split("dptest")
                    if len(r) != 2:
                        print(r, " has wrong serial format - skipping")
                        continue
                    new_encoder_pos = [int(y) for y in [x.split(",")[:-1] for x in r][1]]
                    print(new_encoder_pos)
                    # correcting the uint overflow -> 16383 (14bit max) jump to 0
                    for i in range(4):
                        if self.encoder_pos[i] -uint_overflow_correction[i] - new_encoder_pos[i] > 10000:
                            uint_overflow_correction[i] += 16383
                        if self.encoder_pos[i] - uint_overflow_correction[i] - new_encoder_pos[i] < -10000:
                            uint_overflow_correction[i] -= 16383
                        new_encoder_pos[i] += uint_overflow_correction[i]
                    for i in range(4, 6):
                        # 136 steps for the endeffector encoders
                        new_encoder_pos[i] = abs(new_encoder_pos[i] % (136 * 2))
                    self.encoder_pos = new_encoder_pos
                    #print(self.encoder_pos)
                else:
                    time.sleep(0.01)

    def test_encoder(self):
        res = util.upload_firmware('./firmware/04 encoder read')
        self.assertEqual(res, 0, msg='failed to upload firmware')
        time.sleep(1)
        # move serial connection handling to other thread
        serial_connection_thread = Thread(target=self.handle_serial_connection)
        serial_connection_thread.start()

        time.sleep(3)
        #self.assertEqual(len(self.encoder_pos), 4, msg="getting no data from connection thread")
        start_position = self.encoder_pos
        
        display_interval = 7500
        display_step_size = 200

        try: 
            while serial_connection_thread.is_alive():
                rel_pos = [self.encoder_pos[i] - start_position[i] for i in range(len(self.encoder_pos))]
                print("Move the handles to test the encoders")
                print(rel_pos)
                print(" " * int(display_interval / display_step_size), "|")
                # for j in range(4): # four main encoders
                #     b = True
                #     for i in range(-display_interval, display_interval, display_step_size):
                #         if rel_pos[j] < i and b:
                #             b = False
                #             print("#", end="")
                #         else:
                #             print(" ", end="")
                #     if b:
                #         print("#", end="")
                #     print()
                # print()
                # for j in range(4, 6): # two endeffector encoders
                #     print("#" * (rel_pos[j] // 4))
                # print()
                print("Press CTRL + C to continue")
                time.sleep(0.05)
                os.system('cls' if os.name == 'nt' else 'clear')
        except KeyboardInterrupt:
            self.continue_serial_connection_flag = False

    def test_sync(self):
        res = util.upload_firmware('./firmware/05 move handles in sync')
        self.assertEqual(res, 0, msg='failed to upload firmware')
        time.sleep(1)
        # with serial.Serial(config.COM_PORT, 9600, timeout=1) as ser:
        #     while True:
        #         message = b'dpmotor100234;'
        #         ser.write(message)
                #res = ser.read(1)
                #print(res)
               



# if __name__ == "__main__":
#     h = HardwareTest()
#     #h.test_encoder()
#     #h.test_motor()
#     h.test_sync()