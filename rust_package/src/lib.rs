pub mod demo {
    pub mod echo {
        pub mod v1 {
            include!("gen/demo/echo/v1/demo.echo.v1.rs");
        }
    }
    pub mod notification {
        pub mod v1 {
            include!("gen/demo/notification/v1/demo.notification.v1.rs");
        }
    }
    pub mod user {
        pub mod v1 {
            include!("gen/demo/user/v1/demo.user.v1.rs");
        }
    }
}
