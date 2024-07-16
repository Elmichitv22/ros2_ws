import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from turtlesim.msg import Pose
import time

LIMITE_IZQUIERDO = 4.2
LIMITE_DERECHO = 10.5
LIMITE_SUPERIOR = 10.5
LIMITE_INFERIOR = 4.2
DISTANCIA_MINIMA_PARED = 1.0

class MotionController(Node):

    def __init__(self):
        super().__init__('topics_practice_b')

        for i in range(5, 0, -1):
            print('Application starts in:', i)
            time.sleep(1)
        print('Initiating MotionController')

        self.publisher_ = self.create_publisher(Twist, '/turtle1/cmd_vel', 10)
        self.pose_subscriber_ = self.create_subscription(
            Pose,
            '/turtle1/pose',
            self.pose_callback,
            10
        )
        self.timer_ = self.create_timer(0.1, self.mover_tortuga)
        self.distancia = 0.05
        self.es_perform_spiral = True
        self.current_pose = None
        self.avoiding_wall = False

    def pose_callback(self, msg):
        self.current_pose = msg

    def mover_tortuga(self):
        vel_msg = Twist()

        if self.current_pose is None:
            return

        x = self.current_pose.x
        y = self.current_pose.y

        if self.avoiding_wall:
            vel_msg.linear.x = 0.5
            vel_msg.angular.z = 1.0
            self.get_logger().info("Avoiding wall")
            self.publisher_.publish(vel_msg)
            if LIMITE_IZQUIERDO + DISTANCIA_MINIMA_PARED < x < LIMITE_DERECHO - DISTANCIA_MINIMA_PARED and \
               LIMITE_INFERIOR + DISTANCIA_MINIMA_PARED < y < LIMITE_SUPERIOR - DISTANCIA_MINIMA_PARED:
                self.avoiding_wall = False
            return

        if self.es_perform_spiral:
            vel_msg.linear.x = self.distancia
            vel_msg.angular.z = 1.0
            self.distancia += 0.01
            self.get_logger().info("Drawing spiral")
        else:
            vel_msg.linear.x = 2.0
            vel_msg.angular.z = 0.0
            self.get_logger().info("Going straight")

        self.publisher_.publish(vel_msg)

        if x < LIMITE_IZQUIERDO + DISTANCIA_MINIMA_PARED or x > LIMITE_DERECHO - DISTANCIA_MINIMA_PARED or \
           y < LIMITE_INFERIOR + DISTANCIA_MINIMA_PARED or y > LIMITE_SUPERIOR - DISTANCIA_MINIMA_PARED:
            self.get_logger().info("Avoiding walls")
            self.avoiding_wall = True

        if self.es_perform_spiral:
            if x < LIMITE_IZQUIERDO or x > LIMITE_DERECHO or y > LIMITE_SUPERIOR or y < LIMITE_INFERIOR:
                self.es_perform_spiral = False

def main(args=None):
    rclpy.init(args=args)

    motion_controller = MotionController()

    rclpy.spin(motion_controller)

    motion_controller.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
