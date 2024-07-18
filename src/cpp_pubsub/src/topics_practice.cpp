#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"

using namespace std::chrono_literals;

class MotionController : public rclcpp::Node {
public:
    MotionController()
        : Node("topics_practice"), distance_(0.05), in_spiral_(true) {
        start_countdown();
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        subscription_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, std::bind(&MotionController::pose_callback, this, std::placeholders::_1));
        timer_ = this->create_wall_timer(100ms, std::bind(&MotionController::move_turtle, this));
        current_pose_ = nullptr;
    }

private:
    void start_countdown() {
        for (int i = 5; i > 0; --i) {
            RCLCPP_INFO(this->get_logger(), "Application starts in: %d", i);
            std::this_thread::sleep_for(1s);
        }
        RCLCPP_INFO(this->get_logger(), "Initiating MotionController");
    }

    void pose_callback(const turtlesim::msg::Pose::SharedPtr msg) {
        current_pose_ = msg;
    }

    void move_turtle() {
        if (!current_pose_) return;

        auto vel_msg = geometry_msgs::msg::Twist();
        float x = current_pose_->x;
        float y = current_pose_->y;

        if (in_spiral_) {
            vel_msg.linear.x = distance_;
            vel_msg.angular.z = 2.0;
            distance_ += 0.025;
            RCLCPP_INFO(this->get_logger(), "Drawing spiral");

            if (x < LIMITS.at("left") || x > LIMITS.at("right") || y > LIMITS.at("top") || y < LIMITS.at("bottom")) {
                RCLCPP_INFO(this->get_logger(), "Going straight");
                in_spiral_ = false;
            }
        } else {
            vel_msg.linear.x = 4.0;
            RCLCPP_INFO(this->get_logger(), "Going straight");
        }

        publisher_->publish(vel_msg);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;
    turtlesim::msg::Pose::SharedPtr current_pose_;
    float distance_;
    bool in_spiral_;

    const std::map<std::string, float> LIMITS = {
        {"left", 4.2},
        {"right", 10.5},
        {"top", 10.5},
        {"bottom", 4.2}
    };
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MotionController>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
