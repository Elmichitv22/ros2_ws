#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"

using std::placeholders::_1;

class SpiralPublisher : public rclcpp::Node {
public:
    SpiralPublisher() : Node("spiral_publisher"), distance(0.05) {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtlesim1/turtle1/cmd_vel", 10);
        subscription_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtlesim1/turtle1/pose", 10, std::bind(&SpiralPublisher::pose_callback, this, _1));
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(100), std::bind(&SpiralPublisher::move_turtle, this));
    }

private:
    void pose_callback(const turtlesim::msg::Pose::SharedPtr msg) {
        current_pose = msg;
    }

    void move_turtle() {
        if (!current_pose) {
            return;
        }

        auto vel_msg = geometry_msgs::msg::Twist();

        vel_msg.linear.x = distance;
        vel_msg.angular.z = 2.0;
        distance += 0.025;

        publisher_->publish(vel_msg);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;
    turtlesim::msg::Pose::SharedPtr current_pose;
    double distance;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SpiralPublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
