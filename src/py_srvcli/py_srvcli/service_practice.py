import rclpy
from rclpy.node import Node
from turtlesim.srv import SetPen
from turtlesim.msg import Pose

class ServicePractice(Node):

    def __init__(self):
        super().__init__('service_practice')
        self.cli = self.create_client(SetPen, 'turtle1/set_pen')
        while not self.cli.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('Service not available, waiting again...')
        self.req = SetPen.Request()
        self.subscription = self.create_subscription(
            Pose,
            'turtle1/pose',
            self.listener_callback,
            10)
        self.is_red = False

    def send_request(self, r, g, b):
        self.req.r = r
        self.req.g = g
        self.req.b = b
        self.req.width = 3
        self.req.off = 0
        self.future = self.cli.call_async(self.req)

    def listener_callback(self, msg):
        if msg.x > 5.5:
            if not self.is_red:
                self.get_logger().info('Set pen color to red')
                self.send_request(255, 0, 0)
                self.is_red = True
        else:
            if self.is_red:
                self.get_logger().info('Set pen color to green')
                self.send_request(0, 255, 0)
                self.is_red = False

def main(args=None):
    rclpy.init(args=args)
    service_practice = ServicePractice()
    rclpy.spin(service_practice)
    service_practice.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
