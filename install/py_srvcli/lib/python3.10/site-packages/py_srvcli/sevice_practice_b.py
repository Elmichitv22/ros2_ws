import rclpy
from rclpy.node import Node
from turtlesim.srv import SetPen
from turtlesim.msg import Pose
from geometry_msgs.msg import Twist

class TurtleServicePracticeB(Node):

    def __init__(self):
        super().__init__('service_practice_b')
        self.cli = self.create_client(SetPen, 'turtle1/set_pen')
        while not self.cli.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('Service not available, waiting again...')
        self.req = SetPen.Request()
        self.subscription = self.create_subscription(
            Pose,
            'turtle1/pose',
            self.listener_callback,
            10)
        self.publisher = self.create_publisher(Twist, 'turtle1/cmd_vel', 10)
        self.timer = self.create_timer(0.1, self.move_turtle)
        self.current_pose = None
        self.is_red = False

    def send_request(self, r, g, b):
        self.req.r = r
        self.req.g = g
        self.req.b = b
        self.req.width = 3
        self.req.off = 0
        self.future = self.cli.call_async(self.req)

    def listener_callback(self, msg):
        self.current_pose = msg
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

    def move_turtle(self):
        if self.current_pose is None:
            return

        twist = Twist()
        if (self.current_pose.x < 1.0 or self.current_pose.x > 10.0 or 
            self.current_pose.y < 1.0 or self.current_pose.y > 10.0):
            twist.linear.x = 0.5
            twist.angular.z = 1.0
        else:
            twist.linear.x = 1.0
            twist.angular.z = 0.0

        self.publisher.publish(twist)

def main(args=None):
    rclpy.init(args=args)
    turtle_service_practice_b = TurtleServicePracticeB()
    rclpy.spin(turtle_service_practice_b)
    turtle_service_practice_b.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
