import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from turtlesim.msg import Pose

class SpiralPublisher(Node):
    def __init__(self):
        super().__init__('spiral_publisher')
        self.publisher_ = self.create_publisher(Twist, '/turtlesim1/turtle1/cmd_vel', 10)
        self.create_subscription(Pose, '/turtlesim1/turtle1/pose', self.pose_callback, 10)
        self.create_timer(0.1, self.move_turtle)
        self.distance = 0.05
        self.current_pose = None

    def pose_callback(self, msg):
        self.current_pose = msg

    def move_turtle(self):
        if self.current_pose is None:
            return

        vel_msg = Twist()

        vel_msg.linear.x = self.distance
        vel_msg.angular.z = 2.0
        self.distance += 0.025

        self.publisher_.publish(vel_msg)

def main(args=None):
    rclpy.init(args=args)
    spiral_publisher = SpiralPublisher()
    rclpy.spin(spiral_publisher)
    spiral_publisher.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
