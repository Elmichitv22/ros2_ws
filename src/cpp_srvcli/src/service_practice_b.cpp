#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "turtlesim/srv/set_pen.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"

class TurtleServicePracticeB : public rclcpp::Node {
public:
    TurtleServicePracticeB() : Node("service_practice_b"), is_red_(false) {
        cli_ = this->create_client<turtlesim::srv::SetPen>("turtle1/set_pen");
        while (!cli_->wait_for_service(std::chrono::seconds(1))) {
            RCLCPP_INFO(this->get_logger(), "Service not available, waiting again...");
        }
        subscription_ = this->create_subscription<turtlesim::msg::Pose>(
            "turtle1/pose",
            10,
            std::bind(&TurtleServicePracticeB::listener_callback, this, std::placeholders::_1)
        );
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(100), 
            std::bind(&TurtleServicePracticeB::move_turtle, this)
        );
    }

private:
    void send_request(uint8_t r, uint8_t g, uint8_t b) {
        auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
        request->r = r;
        request->g = g;
        request->b = b;
        request->width = 3;
        request->off = 0;
        auto result = cli_->async_send_request(request);
    }

    void listener_callback(const turtlesim::msg::Pose::SharedPtr msg) {
        current_pose_ = msg;
        if (msg->x > 5.5) {
            if (!is_red_) {
                RCLCPP_INFO(this->get_logger(), "Set pen color to red");
                send_request(255, 0, 0);
                is_red_ = true;
            }
        } else {
            if (is_red_) {
                RCLCPP_INFO(this->get_logger(), "Set pen color to green");
                send_request(0, 255, 0);
                is_red_ = false;
            }
        }
    }

    void move_turtle() {
        if (current_pose_ == nullptr) {
            return;
        }

        auto twist = geometry_msgs::msg::Twist();
        if (current_pose_->x < 1.0 || current_pose_->x > 10.0 || 
            current_pose_->y < 1.0 || current_pose_->y > 10.0) {
            twist.linear.x = 0.5;
            twist.angular.z = 1.0;
        } else {
            twist.linear.x = 1.0;
            twist.angular.z = 0.0;
        }

        publisher_->publish(twist);
    }

    rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr cli_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    turtlesim::msg::Pose::SharedPtr current_pose_;
    bool is_red_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleServicePracticeB>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
