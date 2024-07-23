import sys
import rclpy
from rclpy.node import Node
from example_interfaces.srv import AddTwoInts

class ClientPractice(Node):

    def __init__(self):
        super().__init__('client_practice')
        self.cli = self.create_client(AddTwoInts, 'set_pen_colors')
        while not self.cli.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('Service not available, waiting again...')
        self.req = AddTwoInts.Request()

    def send_request(self, red, green):
        self.req.a = red
        self.req.b = green
        self.future = self.cli.call_async(self.req)
        rclpy.spin_until_future_complete(self, self.future)
        return self.future.result()

def main(args=None):
    rclpy.init(args=args)
    if len(sys.argv) < 3:
        print("Usage: client_practice R G")
        sys.exit(1)

    red = int(sys.argv[1])
    green = int(sys.argv[2])

    client_practice = ClientPractice()
    response = client_practice.send_request(red, green)
    
    if response is not None:
        client_practice.get_logger().info('Pen colors updated successfully with sum: %d' % response.sum)
    else:
        client_practice.get_logger().error('Failed to update pen colors')
    
    client_practice.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
